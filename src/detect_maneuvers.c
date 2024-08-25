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


void detectManeuvers(const tle_storage tle_st, int windowSize, double Sigthresh) {
    printf("Starting maneuver detection...\n");
    printf("initializing detection...\n");
    int nmemb = tle_st.nmemb;
    double meanMotions[nmemb];
    double eccentricity[nmemb];
    double inclination[nmemb];
    printf("Initializing calculation of velocities... \n");
    
    
    // Dynamically allocate memory for the velocities
    printf("Grabbing list of Mean Motion\n");
    int count = 0;
    while (count < nmemb) {
        meanMotions[count] = tle_st.tles[count].line2.meanMotion;
        eccentricity[count] = tle_st.tles[count].line2.eccentricity;
        inclination[count] = tle_st.tles[count].line2.inclination;
        //printf("Difference between calcs of velocity mags: %f\n", computeVectorMagnitude(velocities[count][0],velocities[count][1],velocities[count][2])-velocity_magnitude[count]); 
        //printf("Computed via vector: %f, and computed via orbital elements: %f", computeVectorMagnitude(velocities[count][0],velocities[count][1],velocities[count][2]), velocity_magnitude[count]);
        count++;
    }

    // Compute delta V vectors and their magnitudes

    for (int i = windowSize; i < nmemb - 1; i++) {
        // Create a window of delta V magnitudes
        double window1[windowSize];
        double window2[windowSize];
        double window3[windowSize];
        for (int j = 0; j < windowSize; j++) {
            window1[j] = meanMotions[i - j];
            window2[j] = eccentricity[i - j];
            window3[j] = inclination[i - j];
        }
        // Compute the median of the window
        double median1 = findMedian(window1, windowSize);
        double median2 = findMedian(window2, windowSize);
        double median3 = findMedian(window3, windowSize);
        // Check if the current delta V magnitude exceeds the threshold relative to the median
        double deviation1 = fabs(meanMotions[i] - median1)*1.e5;
        double deviation2 = fabs(eccentricity[i] - median2)*1.e5;
        double deviation3 = fabs(inclination[i] - median3)*1.e4;
        //printf("This is the deviation: %f\n", deviation);
        //printf("This is the deviation: %f\n", deviation);

        if (deviation1 > Sigthresh*6) {
            printf("Potential maneuver detected for mean motion at index %d (Epochyear: %d and Epochday: %f)\n", i + 1 ,tle_st.tles[i].line1.epochYear,tle_st.tles[i].line1.epochDay);
            //printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
        }
        /*
        if (deviation2 > Sigthresh) {
            //printf("deviation %f\n", deviation2);
            printf("Potential maneuver detected for eccentricity at index %d (Epochyear: %d and Epochday: %f)\n", i + 1 ,tle_st.tles[i].line1.epochYear,tle_st.tles[i].line1.epochDay);
            //printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
        }

        if (deviation3 > Sigthresh) {
            printf("Potential maneuver detected for inclination at index %d (Epochyear: %d and Epochday: %f)\n", i + 1 ,tle_st.tles[i].line1.epochYear,tle_st.tles[i].line1.epochDay);
            //printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
        }*/
    }
    printf("End of maneuver detection\n");
}


