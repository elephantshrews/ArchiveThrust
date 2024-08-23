#include "detect_maneuvers.h"
#include <math.h>

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


void detectManeuvers(const tle_storage tle_st, int window_size, double Sigthresh) {
    printf("Starting maneuver detection...\n");
    printf("initializing detection...\n");
    double mean_motions[]
    printf("Initializing calculation of velocities... \n");
    
    
    // Dynamically allocate memory for the velocities

    printf("Allocating memory for velocities...\n");
    double (*velocities)[3] = (double (*)[3])malloc(nmemb * sizeof(*velocities));
    printf("Finished allocating memory\n");

    if (velocities == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);  // Exit if allocation fails
    }
    printf("Memory allocation successfull\n");
    int count = 0;
    printf("Starting computation of velocities...\n");
    while (count < nmemb) {
        computeVelocityFromTLE(&tle_st.tles[count].line2, &velocities[count][0], &velocities[count][1], &velocities[count][2]);
        computeVelocityMagnitudeFromTLE(&tle_st.tles[count].line2, &velocity_magnitude[count]);
        //printf("Difference between calcs of velocity mags: %f\n", computeVectorMagnitude(velocities[count][0],velocities[count][1],velocities[count][2])-velocity_magnitude[count]); 
        //printf("Computed via vector: %f, and computed via orbital elements: %f", computeVectorMagnitude(velocities[count][0],velocities[count][1],velocities[count][2]), velocity_magnitude[count]);
        count++;
    }

    // Compute delta V vectors and their magnitudes
    printf("computing Delta Vs...\n");
    for (int i = 0; i < nmemb - 1; i++) {
        computeDeltaV(velocities[i][0], velocities[i][1], velocities[i][2],
                      velocities[i + 1][0], velocities[i + 1][1], velocities[i + 1][2],
                      &deltaV[i][0], &deltaV[i][1], &deltaV[i][2]);
        //printf("this should be a double: %f\n", deltaV[i][0]);
        deltaVMagnitudes[i] = computeVectorMagnitude(deltaV[i][0], deltaV[i][1], deltaV[i][2]);
        
        
    }
    for (int i = window_size; i < nmemb - 1; i++) {
        // Create a window of delta V magnitudes
        double window[window_size];
        for (int j = 0; j < window_size; j++) {
            window[j] = deltaVMagnitudes[i - j];
        }
        // Compute the median of the window
        double median = findMedian(window, window_size);
        
        // Check if the current delta V magnitude exceeds the threshold relative to the median
        double deviation = fabs(deltaVMagnitudes[i] - median);
        //printf("This is the deviation: %f\n", deviation);

        if (deviation > Sigthresh) {
            printf("Potential maneuver detected at index %d (Epochyear: %d and Epochday: %f)\n", i + 1 ,tle_st.tles[i].line1.epochYear,tle_st.tles[i].line1.epochDay);
            //printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
        }
    }
    printf("End of maneuver detection\n");
}


