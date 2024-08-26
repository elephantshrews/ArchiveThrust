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

bool is_in_list(int index, int *list_of_indices, int indices_count) {
    for (int i = 0; i < indices_count; i++) {
        if (list_of_indices[i] == index) {
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


void detectManeuvers(const tle_storage tle_st, int windowSize, double Sigthresh) {
    printf("Starting maneuver detection...\n");
    printf("initializing detection...\n");
    int nmemb = tle_st.nmemb;
    double meanMotions[nmemb];
    int listOfManeuvers[nmemb];
    printf("Initializing calculation of velocities... \n");
    
    
    // Dynamically allocate memory for the velocities
    printf("Grabbing list of Mean Motion\n");
    int count = 0;
    while (count < nmemb) {
        meanMotions[count] = tle_st.tles[count].line2.meanMotion;
        count++;
    }
    

    // Compute delta V vectors and their magnitudes
    int maneuverCount = 0;

    for (int i = windowSize; i < nmemb - 1; i++) {
        // Create a window of delta V magnitudes
        double window1[windowSize];
        int realSizeOfWindow = 0;
        for (int j = 0; j < windowSize; j++) {
            if (!is_in_list(i-j-1,listOfManeuvers,maneuverCount+1)) {
                window1[j] = meanMotions[i - j - 1];
                //printf("window entry: %f\n", window1[j]);
                //printf("index of potential maneuver in window: %d\n", i-j+1);
                realSizeOfWindow ++;
            }
        }
        double AvFluct1 = findAvFluct(window1, realSizeOfWindow);
        //printf("Average fluctuation: %f\n", AvFluct1);
        // Compute the median of the window
        double median1 = findMedian(window1, realSizeOfWindow);
        double deviation1 = fabs(meanMotions[i] - median1);
        double deviation1Normalized = deviation1/AvFluct1;
        //printf("deviation: %f\n", deviation1);
        //printf("This is the normalized deviation: %f\n", deviation1Normalized);
        if (deviation1Normalized > Sigthresh) {
            printf("Deviation: %f\n", deviation1Normalized);
            printf("Potential maneuver detected for mean motion at index %d (Epochyear: %d and Epochday: %f)\n", i + 1 ,tle_st.tles[i].line1.epochYear,tle_st.tles[i].line1.epochDay);
            //printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
            listOfManeuvers[maneuverCount] = i + 1;
            maneuverCount ++;
        }
    }
    printf("End of maneuver detection\n");
}


