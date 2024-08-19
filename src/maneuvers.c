#include "maneuvers.h"
#include "utils.h"


void detectManeuvers(double velocities[][3], int dataSize,int window_size, double Sigthresh) {
    printf("initializing detection\n");
    double deltaV[dataSize - 1][3];  // Array to store delta V vectors
    double deltaVMagnitudes[dataSize - 1];  // Array to store magnitudes of delta V vectors

    // Compute delta V vectors and their magnitudes
    printf("computing Delta Vs\n");
    for (int i = 0; i < dataSize - 1; i++) {
        printf("count: %d\n", i);
        computeDeltaV(velocities[i][0], velocities[i][1], velocities[i][2],
                      velocities[i + 1][0], velocities[i + 1][1], velocities[i + 1][2],
                      &deltaV[i][0], &deltaV[i][1], &deltaV[i][2]);
        //printf("this should be a double: %f\n", deltaV[i][0]);
        deltaVMagnitudes[i] = computeVectorMagnitude(deltaV[i][0], deltaV[i][1], deltaV[i][2]);
        
        
    }

    // Apply median filter and detect maneuvers
    for (int i = window_size; i < dataSize - 1; i++) {
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
            printf("Potential maneuver detected at index %d (Epoch: %d)\n", i + 1, i + 1);
            printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
        }
    }
}

