#include "detect_maneuvers.h"

void bubbleSort(double *arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap arr[j] and arr[j + 1]
                double temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
// Function to find the median of the array
double findMedian(double *arr, int size) {
    // Create a copy of the array to sort
    double *sortedArr = (double *)malloc(size * sizeof(double));
    memcpy(sortedArr, arr, size * sizeof(double));
    
    // Sort the array using Bubble Sort
    bubbleSort(sortedArr, size);
    
    // Calculate the median
    double median;
    if (size % 2 == 0) {
        median = (sortedArr[size / 2 - 1] + sortedArr[size / 2]) / 2.0;
    } else {
        median = sortedArr[size / 2];
    }
    
    // Free the allocated memory
    free(sortedArr);
    //printf("this is the median: \n%f\n\n", median);
    return median;
}

        //FOR VALIDATION AND COMPARISON WITH MANEUVER LIST

bool IsInList(int index, int *listOfIndices, int indicesCount) {
    for (int i = 0; i < indicesCount; i++) {
        if (listOfIndices[i] == index) {
            return true;
        }
    }
    return false;
}
double findAvFluct(const double *arr, int realSizeOfWindow){
    double diff = 0;
    for (int j = 0; j<realSizeOfWindow-1; j++){
        diff += fabs(arr[j+1]-arr[j]);
        //printf("this is the diff: %f\n", fabs(arr[j+1]-arr[j]));
    }
    double averageFluctuation = diff/(realSizeOfWindow-1);
    return averageFluctuation;
}
void fitFadingMemoryPolynomial(const double *epochDays, const double *orbitParams, int windowSize, double *coefficients,double chisq) {
    // Step 1: Set up the matrices and vectors needed by GSL
    gsl_matrix *X = gsl_matrix_alloc(windowSize, POLY_DEGREE + 1);
    gsl_vector *y = gsl_vector_alloc(windowSize);
    gsl_vector *c = gsl_vector_alloc(POLY_DEGREE + 1);
    gsl_matrix *cov = gsl_matrix_alloc(POLY_DEGREE + 1, POLY_DEGREE + 1);

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


void detectManeuvers(const tle_storage *tleSt) {
    printf("Starting maneuver detection...\n");
    printf("initializing detection...\n");
    int nmemb = tleSt->nmemb;
    double meanMotions[nmemb];
    //int listOfManeuversMeanMotion[nmemb];
    printf("Initializing calculation of velocities... \n");
    
    
    // Dynamically allocate memory for the velocities
    printf("Grabbing list of Mean Motion\n");
    int count = 0;
    while (count < nmemb) {
        meanMotions[count] = tleSt->tles[count].line2.meanMotion;
        count++;
    }
    

    // Compute delta V vectors and their magnitudes
    int maneuverCountMeanMotion = 0;
    for (int i = WindowSize; i < nmemb - 1; i++) {
        // Create a window of delta V magnitudes
        double windowMeanMotion[WindowSize];
        double epochDays[WindowSize];
        double epochYears[WindowSize];
        int realSizeOfWindowMeanMotion = 0;
        for (int j = 0; j < WindowSize; j++) {
            windowMeanMotion[j] = meanMotions[i - WindowSize + j];
            epochDays[j] = tleSt->tles[i - WindowSize + j].line1.epochDay;
            epochYears[j] = tleSt->tles[i - WindowSize + j].line1.epochYear;
            epochDays[j] += 365*(epochYears[j]-epochYears[0]);
            epochDays[j] -= epochDays[0]; 
                //printf("window entry: %f\n", window1[j]);
                //printf("index of potential maneuver in window: %d\n", i-j+1);
            realSizeOfWindowMeanMotion ++;
        }  
        double coefficients[POLY_DEGREE + 1];
        double chisq=1;
        fitFadingMemoryPolynomial(epochDays, windowMeanMotion, WindowSize, coefficients, chisq);
        double fittedValue = 0;
        for (int k=0; k<POLY_DEGREE+1; k++) {
            fittedValue += coefficients[k]*pow(epochDays[i]+365*(epochYears[i]-epochYears[i-WindowSize]),k);
            
        }

        double AvFluctMeanMotion = findAvFluct(windowMeanMotion, realSizeOfWindowMeanMotion);
        if (AvFluctMeanMotion<1.e-7){
            AvFluctMeanMotion = 1.e-5;
        }
        
        //double medianMeanMotion = findMedian(windowMeanMotion, realSizeOfWindowMeanMotion);
        double deviationMeanMotion = fabs(meanMotions[i] - fittedValue);
        double deviationMeanMotionNormalized = deviationMeanMotion/AvFluctMeanMotion;
        


        if (deviationMeanMotionNormalized > sigmaThreshold) {
            printf("deviation: %f and average fluctuation: %f\n", deviationMeanMotion, AvFluctMeanMotion);
            printf("Deviation: %f\n", deviationMeanMotionNormalized);
            printf("Potential maneuver detected for mean motion at index %d (Epochyear: %d and Epochday: %f)\n", i + 1 ,tleSt->tles[i].line1.epochYear,tleSt->tles[i].line1.epochDay);
            //printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
            //listOfManeuversMeanMotion[maneuverCountMeanMotion] = i + 1;
            maneuverCountMeanMotion ++;
        }

    }
    printf("End of maneuver detection\n");
}


