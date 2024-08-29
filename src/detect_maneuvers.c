/*                                                                          
* ArchiveThrust - detect_maneuvers.c
*
* Authors: Michiel Snoeken & Freddy Spaulding
* Created in 2024
* GNU General Public License
*
* 
*/

#include "detect_maneuvers.h"

//Place orbital parameters from a list of TLEs in a tle storage into seperate arrays
void _extractOrbParams(const tlePermanentStorage *tle_st, double *epochYears, double *epochDays, double *meanMotions, double *inclinations, double *eccentricities) {
    int nmemb = tle_st->nmemb;
    for (int i = 0; i < nmemb; i++) {
        epochYears[i] = tle_st->tles[i].line1.epochYear;
        epochDays[i] = tle_st->tles[i].line1.epochDay;
        meanMotions[i] = tle_st->tles[i].line2.meanMotion;
        inclinations[i] = tle_st->tles[i].line2.inclination;
        eccentricities[i] = tle_st->tles[i].line2.eccentricity;
    }
}



//Calculate the average fluctuation of a data set with respect to a fitted data set
double _findAvFluct(const double *data, const double *fittedData, int realSizeOfWindow){
    double diff = 0;
    for (int j = 0; j<realSizeOfWindow; j++){
        diff += fabs(data[j]-fittedData[j]);
        //printf("this is the diff: %f\n", fabs(arr[j+1]-arr[j]));
    }
    double averageFluctuation = diff/(realSizeOfWindow);
    return averageFluctuation;
}

/*
 * Function Description:
 * Takes in a windowlength dataset for x values (Epoch days) 
 * and y values (array for one orbital parameter).
 * The gsl library fits the y values to a polynomial of POLY_DEGREE. 
 * The weight of data points further back in time are weighed 
 * exponentially less via the FADE_FACTOR
 * The gsl library then gives the coeffitients of the fitted polynomial
*/
void _fitFadingMemoryPolynomial(const double *epochDays, const double *orbitParams, int windowSize, double *coefficients) {
    // Step 1: Set up the matrices and vectors needed by GSL
    gsl_matrix *X = gsl_matrix_alloc(windowSize, POLY_DEGREE + 1);
    gsl_vector *y = gsl_vector_alloc(windowSize);
    gsl_vector *c = gsl_vector_alloc(POLY_DEGREE + 1);
    gsl_matrix *cov = gsl_matrix_alloc(POLY_DEGREE + 1, POLY_DEGREE + 1);
    double chisq;
    // Step 2: Fill in the design matrix X and vector y with the weighted data
    for (int i = 0; i < windowSize; i++) {
        double fadeWeight = pow(FADE_FACTOR, windowSize - i - 1); // Apply fading factor
        double x = epochDays[i];// /windowSize;
        double yi = orbitParams[i];// /windowSize;
        for (int j = 0; j <= POLY_DEGREE; j++) {
            gsl_matrix_set(X, i, j, fadeWeight * pow(x, j));
        }
        gsl_vector_set(y, i, fadeWeight * yi);
    }

    // Step 3: Perform the linear least squares fit using GSL
    gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(windowSize, POLY_DEGREE + 1);
    gsl_multifit_linear(X, y, c, cov, &chisq, work);
    // Step 4: Extract the coefficients
    for (int i = 0; i <= POLY_DEGREE; i++) {
        coefficients[i] = gsl_vector_get(c, i);
        //printf("These are the coefficients: %d: %f\n", i, coefficients[i]);
    }

    // Step 5: Free allocated memory
    gsl_multifit_linear_free(work);
    gsl_matrix_free(X);
    gsl_vector_free(y);
    gsl_vector_free(c);
    gsl_matrix_free(cov);
}


/*
 * Function description:
 * This function takes in the epoch data and
 * data of an orbital parameter of choice.
 * It takes an initial window of the data 
 * and fits it to a polynomial. The resulting
 * coeffients are used to predict the next value
 * of the orbital parameter outside the window.
 * The prediction is compared with the actual data 
 * point. The deviation is calculated and normalized
 * with respect to the average fluctuation.
 * If the deviation is higher than the sigma
 * threshold, it is cathegorized as a maneuver.
*/
void _singleParamDetection(const double *epochYears, const double *epochDays, const double *orbitParams, int nmemb) {
    printf("Starting maneuver detection...\n");
    //For each i in orbitParams create and fit window of previous entries
    for (int i = WindowSize; i < nmemb - 1; i++) {
        //Prepare data to pass to polynomial fitting function
        double windowOrbitParams[WindowSize] = {0};
        double epochDaysWindow[WindowSize+1] = {0};
        double beginningDay = epochDays[i - WindowSize ]; 
        double epochYearsWindow[WindowSize+1];
        for (int j = 0; j < WindowSize; j++) {
            windowOrbitParams[j] = orbitParams[i - WindowSize + j];
            epochDaysWindow[j] = epochDays[i - WindowSize + j] - beginningDay;
            epochYearsWindow[j] = epochYears[i - WindowSize + j];
            epochDaysWindow[j] += 366*(epochYearsWindow[j]-epochYearsWindow[0]);

        }  
        double coefficients[POLY_DEGREE + 1];
        //Calculate fitting coefficients
        _fitFadingMemoryPolynomial(epochDays, windowOrbitParams, WindowSize, coefficients);
        epochYearsWindow[WindowSize] = epochYears[i]; 
        epochDaysWindow[WindowSize] = epochDays[i]-beginningDay+366*(epochYearsWindow[WindowSize]-epochYearsWindow[0]);
        
        // Prepare fitted data for calculation of average fluctuation
        double fittedValues[WindowSize+1] = {0};
        for (int l=0; l<WindowSize+1; l++){
            for (int k=0; k<POLY_DEGREE+1; k++) {
                fittedValues[l] += coefficients[k]*pow(epochDays[l],k);
            }
        }
        double AvFluct = _findAvFluct(windowOrbitParams, fittedValues, WindowSize);
        //Protect from division by zero
        if (AvFluct<1.e-7){
            AvFluct = 1.e-5;
        }
        
        //Calculate deviation and normalize
        double deviation = fabs(orbitParams[i] - fittedValues[WindowSize]);
        double deviationNormalized = deviation/AvFluct;
        

        //Print if a maneuver is detected
        if (deviationNormalized > sigmaThreshold) {
            printf("Potential maneuver detected for mean motion at index %d (Epochyear: %f and Epochday: %f)\n", i + 1 ,epochYears[i],epochDays[i]);

        }

    }
    printf("End of maneuver detection\n");
}

/*
 * Function description:
 * Ultimately allows for detection
 * of maneuvers with respect to different 
 * orbital parameters.
 * Instead of meanMotions as an argument
 * in singleParamDetection any other can 
 * be given.
*/
void detectManeuvers(const tlePermanentStorage *tleSt) {
    int nmemb = tleSt->nmemb;
    double epochDays[nmemb];
    double epochYears[nmemb];
    double meanMotions[nmemb];
    double inclinations[nmemb];
    double eccentricities[nmemb];

    _extractOrbParams(tleSt, epochYears, epochDays, meanMotions, inclinations,eccentricities);
    _singleParamDetection( epochYears, epochDays, meanMotions, nmemb);

}