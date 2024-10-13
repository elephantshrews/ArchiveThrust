
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

int sigmathresholds[] = {10,15,15,20,20,20};
//Place orbital parameters from a list of TLEs in a tle storage into seperate arrays
void _extractOrbParams(const TleStor *tle_st, double *epochYears, double *epochDays, double *meanMotions, double *inclinations, double *eccentricities, double *argPerigee, double *raan, double *meanAnomaly) {
    int nmemb = tle_st->nmemb;
    double argPerigeeraw[nmemb];
    double raanraw[nmemb]; 
    double meanAnomalyraw[nmemb];
    for (int i = 0; i < nmemb; i++) {
        epochYears[i] = tle_st->tles[i].line1.epochYear;
        epochDays[i] = tle_st->tles[i].line1.epochDay;
        meanMotions[i] = tle_st->tles[i].line2.meanMotion;
        inclinations[i] = tle_st->tles[i].line2.inclination;
        eccentricities[i] = tle_st->tles[i].line2.eccentricity;
        argPerigeeraw[i] = tle_st->tles[i].line2.argPerigee;
        raanraw[i] = tle_st->tles[i].line2.raan;
        meanAnomalyraw[i] = tle_st->tles[i].line2.meanAnomaly;
    }
    createMod360Degrees(argPerigeeraw, argPerigee, nmemb/2);
    createMod360Degrees(raanraw, raan, nmemb/2);
    createMod360Degrees(meanAnomalyraw, meanAnomaly, nmemb/2);
}

void createMod360Degrees(double *orbitalParams, double *ModOrbitParams, int nmemb){
    int windingNumber = 0;
    ModOrbitParams[0] = orbitalParams[0];
    for (int i = 1; i < nmemb; i++){
        if (orbitalParams[i-1]<20 && orbitalParams[i]>340){
            windingNumber--;
        }
        if (orbitalParams[i-1]>340 && orbitalParams[i]<20){
            windingNumber++;
        }
        ModOrbitParams[i] = orbitalParams[i] + windingNumber*360;
    }
}

//Calculate the average fluctuation of a data set with respect to a fitted data set
double _findAvFluct(const double *data, const double *fittedData, int Windowsize){
    double diff = 0;
    for (int j = 0; j<Windowsize; j++){
        diff += fabs(data[j]-fittedData[j]);
        //printf("this is the diff: %f\n", fabs(arr[j+1]-arr[j]));
    }
    double averageFluctuation = diff/(Windowsize);
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
void _fitFadingMemoryPolynomial(const double *epochDays, const double *orbitParams, int windowSize, double *coefficients, int poly_degree) {
    // Step 1: Set up the matrices and vectors needed by GSL
    gsl_matrix *X = gsl_matrix_alloc(windowSize, poly_degree + 1);
    gsl_vector *y = gsl_vector_alloc(windowSize);
    gsl_vector *c = gsl_vector_alloc(poly_degree + 1);
    gsl_matrix *cov = gsl_matrix_alloc(poly_degree + 1, poly_degree + 1);
    double chisq;
    // Step 2: Fill in the design matrix X and vector y with the weighted data
    for (int i = 0; i < windowSize; i++) {
        double fadeWeight = pow(FADE_FACTOR, windowSize - i - 1); // Apply fading factor
        double x = epochDays[i];// /windowSize;
        double yi = orbitParams[i];// /windowSize;
        for (int j = 0; j <= poly_degree; j++) {
            gsl_matrix_set(X, i, j, fadeWeight * pow(x, j));
        }
        gsl_vector_set(y, i, fadeWeight * yi);
    }

    // Step 3: Perform the linear least squares fit using GSL
    gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(windowSize, poly_degree + 1);
    gsl_multifit_linear(X, y, c, cov, &chisq, work);
    // Step 4: Extract the coefficients
    for (int i = 0; i <= poly_degree; i++) {
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


void createNormalizedWindow(double *windowOrbitalParams, int windowSize, double *normalizedWindow, double *meanValue){
    double value = 0.;
    for (int i = 0; i< windowSize; i++){
        value += windowOrbitalParams[i];
    }
    
    value /= windowSize;
    value /= 0.1;
    *meanValue = value;
    for (int i = 0; i< windowSize; i++) {
        normalizedWindow[i] = windowOrbitalParams[i]/value;
    }

}


void classifyManeuver(Maneuver *maneuver){
    //printf("this is the orbitalpüaram count: %d, %d, %d, %d, %d, %d\n", maneuver->affectedParams[0], maneuver->affectedParams[1],maneuver->affectedParams[2], maneuver->affectedParams[3], maneuver->affectedParams[4], maneuver->affectedParams[5]);
    if ((maneuver->affectedParams[0]) && (maneuver->affectedParams[1] || maneuver->affectedParams[4])) {
        if (maneuver->fluctuations[0]>2*sigmathresholds[0] && (maneuver->fluctuations[1]>2*sigmathresholds[1] || maneuver->fluctuations[4]>2*sigmathresholds[4])) {
            maneuver->confidenceLevel += log(maneuver->fluctuations[0])/20;
            maneuver->confidenceLevel += log(maneuver->fluctuations[1])/20;
            maneuver->confidenceLevel += log(maneuver->fluctuations[4])/20;
            maneuver->maneuverType[1] = STATION_KEEPING;
            maneuver->maneuverType[0] = OUT_OF_PLANE;
        }
        else {                      // If one or more of the mutually exclusive params has a small fluctuation, we will discard the ones that have less significance
            if (maneuver->affectedParams[1]) {
                if (maneuver->fluctuations[0]>maneuver->fluctuations[1]) {
                    maneuver->affectedParams[1] = false;
                    maneuver->affectedParams[4] = false;
                }
                else {
                    maneuver->affectedParams[0] = false;
                    }
            }
            else {
                if (maneuver->fluctuations[0]>maneuver->fluctuations[4]) {
                    maneuver->affectedParams[1] = false;
                    maneuver->affectedParams[4] = false;
                }
                else {
                    maneuver->affectedParams[0] = false;
                    }
            }

        }
                

    }
    if (maneuver->affectedParams[0] && !maneuver->affectedParams[1] && !maneuver->affectedParams[4]) {
        maneuver->maneuverType[0] =  IN_PLANE;
        maneuver->confidenceLevel += log(maneuver->fluctuations[0])/20;
        if (maneuver->affectedParams[3]) {
            maneuver->confidenceLevel += log(maneuver->fluctuations[3])/20;
            maneuver->maneuverType[1] = PERIGEE_APOGEE_CHANGE;
        } 
        else if ( maneuver->affectedParams[2]) {
            maneuver->confidenceLevel += log(maneuver->fluctuations[2])/20;
            maneuver->maneuverType[1] = DRAG_COMPENSATION;
        }
        else{
            maneuver->maneuverType[1] = ORBIT_RAISING_LOWERING;
        }
    }
    else if (!maneuver->affectedParams[0] && (maneuver->affectedParams[1] || maneuver->affectedParams[4])) {
        maneuver->confidenceLevel += log(maneuver->fluctuations[1])/20;
        maneuver->confidenceLevel += log(maneuver->fluctuations[4])/20;
        if (!maneuver->affectedParams[2]&& !maneuver->affectedParams[3]){
            maneuver->maneuverType[0] = OUT_OF_PLANE;
            maneuver->maneuverType[1] = PLANE_CHANGE;
        }
        else {
            maneuver->confidenceLevel += log(maneuver->fluctuations[2])/20;
            maneuver->confidenceLevel += log(maneuver->fluctuations[3])/20;
            maneuver->maneuverType[0] = OUT_OF_PLANE;
            maneuver->maneuverType[1] = STATION_KEEPING;
        }
    }
    else if (!maneuver->affectedParams[0] && !maneuver->affectedParams[1] && !maneuver->affectedParams[4] && !maneuver->affectedParams[2] && !maneuver->affectedParams[3] && maneuver->affectedParams[5] ) {
        maneuver->confidenceLevel += log(maneuver->fluctuations[5])/20; 
        maneuver->maneuverType[0] = IN_PLANE;
        maneuver->maneuverType[1] = PHASING;
    }
    else {
        maneuver->confidenceLevel += 0.2;
        maneuver->maneuverType[0] = OUT_OF_PLANE;
        maneuver->maneuverType[1] = STATION_KEEPING;
    }
    //printf("This is the confidence level: %f\n", maneuver->confidenceLevel);
    if (!maneuver->confidenceLevel<1){
        maneuver->confidenceLevel = 0.5; 
    }
    
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
// Helper function to check if two maneuvers are close enough (within 4 or 5 TLEs)
bool isCloseEnough(double epochDay1, double epochDay2, int threshold) {
    return fabs(epochDay1 - epochDay2) <= threshold;
}

void _singleParamDetection(const TleStor *tleSt, int nmemb, Maneuver *detectedManeuvers) {

    printf("Starting maneuver detection...\n");
    int maxWindowSize = MAX_WINDOW_SIZE;
    int minWindowSize = MIN_WINDOW_SIZE;
    int maxPolyDegree = MAX_POLY_DEGREE;
    int maneuverGroupingThreshold = 5; // Threshold of 5 TLEs

    // Arrays for orbital parameters
    double epochDays[nmemb];
    double epochYears[nmemb];
    double meanMotions[nmemb];
    double inclinations[nmemb];
    double eccentricities[nmemb];
    double argPerigee[nmemb];
    double raan[nmemb]; 
    double meanAnomaly[nmemb];
    
    // Extract orbital parameters
    _extractOrbParams(tleSt, epochYears, epochDays, meanMotions, inclinations, eccentricities, argPerigee, raan, meanAnomaly);
    
    double *params[] = {meanMotions, inclinations, eccentricities, argPerigee, raan, meanAnomaly};
    const char *paramNames[] = {"Mean Motion", "Inclination", "Eccentricity", "Arg Perigee", "RAAN", "Mean Anomaly"};
    // Independent window size and polynomial degree for each parameter
    int windowSize[6] = {INITIAL_WINDOW_SIZE, INITIAL_WINDOW_SIZE, INITIAL_WINDOW_SIZE, INITIAL_WINDOW_SIZE, INITIAL_WINDOW_SIZE, INITIAL_WINDOW_SIZE};
    int pol_deg[6] = {1, 1, 1, 1, 1, 1}; // Initial polynomial degree
    double increaseCL = 0.0;
    // Array to store detected maneuvers
    // Pre-allocate space for up to 100 maneuvers
    int maneuverCount = 0;

    for (int i = INITIAL_WINDOW_SIZE; i < nmemb - 1; i++) {
        if (epochYears[i]<epochYears[i-1]){
            break;
        }
        for (int k = 0; k < 6; k++) {  // Loop through each parameter
            double sigmaThreshold = sigmathresholds[k];
            // Dynamically allocate memory for the window and fitted values
            double *windowOrbitParams = (double *)malloc(windowSize[k] * sizeof(double));
            double *epochDaysWindow = (double *)malloc((windowSize[k] + 1) * sizeof(double));
            double *fittedValues = (double *)malloc((windowSize[k] + 1) * sizeof(double));
            double beginningDay = epochDays[i - windowSize[k]];
            double *epochYearsWindow = (double *)malloc((windowSize[k] + 1) * sizeof(double));
            double *NormalizedWindow = (double *)malloc(windowSize[k] * sizeof(double));
            double meanValue = 0.0 ;
            // Fill the window arrays
            for (int j = 0; j < windowSize[k]; j++) {
                windowOrbitParams[j] = params[k][i - windowSize[k] + j];
                epochDaysWindow[j] = epochDays[i - windowSize[k] + j] - beginningDay;
                epochYearsWindow[j] = epochYears[i - windowSize[k] + j];

                // Adjust for leap years
                int year = (int)round(epochDaysWindow[0]);
                if (year % 4 == 0) {
                    epochDaysWindow[j] += 366 * (epochYearsWindow[j] - epochYearsWindow[0]);
                } else {
                    epochDaysWindow[j] += 365 * (epochYearsWindow[j] - epochYearsWindow[0]);
                }
            }
            if (k== 3|| k==4 ||k==5) {
                createNormalizedWindow(windowOrbitParams, windowSize[k], NormalizedWindow, &meanValue);
            }
            // Optimize polynomial degree for this parameter
            double AvFluct = 0;
            while (pol_deg[k] <= maxPolyDegree) {
                double coefficients[pol_deg[k] + 1];
                memset(fittedValues, 0, (windowSize[k] + 1) * sizeof(double));

                // Fit polynomial to data
                
                if (k== 0|| k==1 ||k==2) {
                    _fitFadingMemoryPolynomial(epochDaysWindow, windowOrbitParams, windowSize[k], coefficients, pol_deg[k]);
                }
                if (k== 3|| k==4 ||k==5) {
                    _fitFadingMemoryPolynomial(epochDaysWindow, NormalizedWindow, windowSize[k], coefficients, pol_deg[k]);
                }
                epochYearsWindow[windowSize[k]] = epochYears[i];
                epochDaysWindow[windowSize[k]] = epochDays[i] - beginningDay + 366 * (epochYearsWindow[windowSize[k]] - epochYearsWindow[0]);

                // Calculate fitted values for this window
                for (int l = 0; l < windowSize[k] + 1; l++) {
                    for (int m = 0; m <= pol_deg[k]; m++) {
                        fittedValues[l] += coefficients[m] * pow(epochDaysWindow[l], m);
                    }
                }

                if (k== 0|| k==1 ||k==2) {
                    AvFluct = _findAvFluct(windowOrbitParams, fittedValues, windowSize[k]);
                }
                
                if (k== 3|| k==4 ||k==5) {
                    AvFluct = _findAvFluct(NormalizedWindow, fittedValues, windowSize[k]);
                }

                // Stop if fluctuation is low enough or max polynomial degree is reached
                if (AvFluct < 5.e-4 || pol_deg[k] == maxPolyDegree) {
                    break;
                }
                pol_deg[k]++;
            }
            int oldwindowSize = windowSize[k];
            // Refine window size based on the fluctuation for this parameter
            if (AvFluct < 5.e-4 && windowSize[k] < maxWindowSize) {
                windowSize[k]++;
            } else if (AvFluct > 5.e-4 && windowSize[k] > minWindowSize) {
                windowSize[k]--;
            }
            if (AvFluct == 0.0) { 
                AvFluct = 5.e-9;
            }
            // Calculate normalized deviation
            double deviation;
            double deviationNormalized;
            if (k== 3|| k==4 ||k==5) {
                deviation = fabs(params[k][i]/ meanValue - fittedValues[oldwindowSize]);
                deviationNormalized = fabs(params[k][i]/ meanValue - fittedValues[oldwindowSize])/AvFluct;
            }
            if (k== 0|| k==1 ||k==2) {
                deviation = fabs(params[k][i] - fittedValues[oldwindowSize]);
                deviationNormalized = fabs(params[k][i] - fittedValues[oldwindowSize])/AvFluct;

            }

            //printf("this is the orbital param value %f and the fitted value4: %f\n", params[k][i]/meanValue, fittedValues[oldwindowSize]);
            //printf("average fluct %f\n", AvFluct);
            //printf("deviation %f and ormalized deviation: %f for param: %s\n",deviation, deviationNormalized, paramNames[k]);

            // Detect potential maneuver
            if (deviationNormalized > sigmaThreshold && deviationNormalized < 100) {
                //printf("Potential maneuver detected for %s in the year %f on day %f\n", paramNames[k], epochYears[i], epochDays[i]);
                if (maneuverCount == 0) {
                    Maneuver newManeuver = {epochDays[maneuverCount], epochDays[i], epochYears[i], {false, false, false, false, false, false}, {0,0,0,0,0,0},-1,-0.1};
                    newManeuver.affectedParams[k] = true;

                    //printf("This should now be true: %d\n",newManeuver.affectedParams[k]);
                    newManeuver.fluctuations[k] = deviationNormalized;
                    detectedManeuvers[maneuverCount] = newManeuver;
                    maneuverCount++;
                }
                else {
                // Check if this maneuver is close to a previous maneuver
                    bool maneuverGrouped = false;
                    if (isCloseEnough(epochDays[i], detectedManeuvers[maneuverCount-1].startEpochDay, maneuverGroupingThreshold)) {
                        //printf("Is close enough at the maneuvercount: %d\n", maneuverCount);
                            // Group the current detection into this maneuver
                        if (!detectedManeuvers[maneuverCount-1].affectedParams[k]){
                            detectedManeuvers[maneuverCount-1].endEpochDay = epochDays[i];
                            detectedManeuvers[maneuverCount-1].affectedParams[k] = true;
                            //printf("This should now be true: %d\n",detectedManeuvers[maneuverCount].affectedParams[k]);
                            detectedManeuvers[maneuverCount-1].fluctuations[k] = deviationNormalized;
                            maneuverGrouped = true;
                        }
                        else{
                            maneuverGrouped = true;
                        }
                    }


                    // If not grouped, create a new maneuver entry
                    if (!maneuverGrouped) {
                        //printf("This is the maneuver count right before the new maneuver is created: %d\n", maneuverCount);
                        classifyManeuver(&detectedManeuvers[maneuverCount-1]);
                        //printf("This is the maneuver type: %d and %d\n", detectedManeuvers[maneuverCount].maneuverType[0],detectedManeuvers[maneuverCount].maneuverType[1]);
                        //printf("On the day: %f and year %f\n", detectedManeuvers[maneuverCount].startEpochDay, detectedManeuvers[maneuverCount].epochYear);
                        Maneuver newManeuver = {epochDays[i], epochDays[i], epochYears[i], {false, false, false, false, false, false}, {0,0,0,0,0,0}, -1, -0.1};
                        newManeuver.affectedParams[k] = true;
                        //printf("This should now be true: %d\n",newManeuver.affectedParams[k]);
                        newManeuver.fluctuations[k] = deviationNormalized;
                        detectedManeuvers[maneuverCount] = newManeuver;
                        maneuverCount++;
                        //printf("Maneuver count after the maneuver was created: %d\n", maneuverCount);
                        //printf("This should also be 1: %d\n", detectedManeuvers[maneuverCount].affectedParams[k]);
                    }
                }
            }

            // Free dynamically allocated memory
            free(windowOrbitParams);
            free(epochDaysWindow);
            free(fittedValues);
            free(epochYearsWindow);
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


void detectManeuvers(const TleStor *tleSt, Maneuver *detectedManeuvers){

    int nmemb = tleSt->nmemb;
    
    
    _singleParamDetection(tleSt, nmemb, detectedManeuvers);
    
}
