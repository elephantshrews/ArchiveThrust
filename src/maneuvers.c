#include "maneuvers.h"
#include <math.h>



//This shouls only be in the freddy branch!!
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


void computeVelocityFromTLE(const TLE_Line2 *tle2, double *v_x, double *v_y, double *v_z) {
    // Step 1: Convert mean motion to semi-major axis (mean motion is in revs per day)
    double mean_anomaly = tle2->meanAnomaly; 
    double ecc = tle2->eccentricity;
    double inc = tle2->inclination;
    double raan = tle2->raan;
    //double arg_perigee = tle2->argPerigee;
    double mean_motion = tle2->meanMotion;
    double n = mean_motion * 2 * 3.14159 / 86400.0; // Convert to rad/s
    double a = pow(MU / (n * n), 1.0 / 3.0);  // Semi-major axis in km
    // Step 2: Solve Kepler's equation to get the eccentric anomaly E
    double E = mean_anomaly;  // Initial guess (could iterate for better accuracy)
    double delta = 1e-6;

    //Iterative method (Newton's method) to compute eccentricity anomaly for step 2
    while (fabs(mean_anomaly - (E - ecc * sin(E))) > delta) {
        E = E - (E - ecc * sin(E) - mean_anomaly) / (1 - ecc * cos(E));
    }
    
    // Step 3: Compute the true anomaly (angle between periapsis direction and satellite's position)
    double true_anomaly = 2 * atan2(sqrt(1 + ecc) * sin(E / 2), sqrt(1 - ecc) * cos(E / 2));
    
    // Step 4: Compute position and velocity in orbital plane
         //double r = a * (1 - ecc * cos(E)); // Distance
    double p = a * (1 - ecc * ecc);    // Semi-latus rectum (related to size and shape of orbit)
    double v_r = sqrt(MU / p) * ecc * sin(true_anomaly);  // Radial velocity component
    double v_t = sqrt(MU / p) * (1 + ecc * cos(true_anomaly));  // Tangential velocity component

    // Position in the orbital plane
    /*
    double r_x = r * cos(true_anomaly);
    double r_y = r * sin(true_anomaly);
    */
    // Step 5: Convert to ECI coordinates using orbital parameters
    // Simplified rotation (for demonstration purposes; need full rotation matrix in actual implementation)
    *v_x = v_r * cos(raan) - v_t * sin(raan) * cos(inc);
    *v_y = v_r * sin(raan) + v_t * cos(raan) * cos(inc);
    *v_z = v_t * sin(inc);
    //printf("this should be a velocity comp.: %f\n ", *v_z);
}
void computeDeltaV(double vx1, double vy1, double vz1, double vx2, double vy2, double vz2, double *delvx, double *delvy, double *delvz) {
     *delvx = vx2 - vx1 ;
     *delvy = vy2 - vy1 ;
     *delvz = vz2 - vz1 ;
        
}
double computeVectorMagnitude(double x, double y, double z) {
    double mag = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
    return mag;
}


//Realizing that I can incorporate this step into the compute velocity from TLE function
double (*listOfVelocities(const tle_storage tle_st))[3] {
    int nmemb = tle_st.nmemb;
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
        count++;
    }
    printf("Finished calculation of velocities\n");

    return velocities;
}


void detectManeuvers(double velocities[][3], int dataSize,int window_size, double Sigthresh) {
    printf("Starting maneuver detection...\n");
    printf("initializing detection...\n");
    double deltaV[dataSize - 1][3];  // Array to store delta V vectors
    double deltaVMagnitudes[dataSize - 1];  // Array to store magnitudes of delta V vectors

    // Compute delta V vectors and their magnitudes
    printf("computing Delta Vs...\n");
    for (int i = 0; i < dataSize - 1; i++) {
        printf("count: %d\n", i);
        computeDeltaV(velocities[i][0], velocities[i][1], velocities[i][2],
                      velocities[i + 1][0], velocities[i + 1][1], velocities[i + 1][2],
                      &deltaV[i][0], &deltaV[i][1], &deltaV[i][2]);
        //printf("this should be a double: %f\n", deltaV[i][0]);
        deltaVMagnitudes[i] = computeVectorMagnitude(deltaV[i][0], deltaV[i][1], deltaV[i][2]);
        
        
    }



    int lastManeuverIndex = 0;  // Keeps track of where the last maneuver occurred

    for (int i = window_size; i < dataSize - 1; i++) {
        // Calculate the size of the current window, which resets after a maneuver
        int currentWindowSize = i - lastManeuverIndex;
        if (currentWindowSize > window_size) {
            currentWindowSize = window_size;  // Limit the window size to the predefined window_size
    }

    // Create a window of delta V magnitudes from the last maneuver point
        double window[currentWindowSize];
        for (int j = 0; j < currentWindowSize; j++) {
            window[j] = deltaVMagnitudes[i - j];
        }

        // Compute the median of the window
        double median = findMedian(window, currentWindowSize);

        // Check if the current delta V magnitude exceeds the threshold relative to the median
        double deviation = fabs(deltaVMagnitudes[i] - median);
        //printf("This is the deviation: %f\n", deviation);

        if (deviation > Sigthresh) {
            // Maneuver detected, reset the window
            printf("Potential maneuver detected at index %d (Epoch: %d)\n", i + 1, i + 1);
            printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);

            // Reset the start of the window to the current index
            lastManeuverIndex = i;
        }
        
    }
    printf("End of maneuver detection\n");
}



/*
void detectManeuvers(double velocities[][3], int dataSize,int window_size, double Sigthresh) {
    printf("Starting maneuver detection...\n");
    printf("initializing detection...\n");
    double deltaV[dataSize - 1][3];  // Array to store delta V vectors
    double deltaVMagnitudes[dataSize - 1];  // Array to store magnitudes of delta V vectors

    // Compute delta V vectors and their magnitudes
    printf("computing Delta Vs...\n");
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
    printf("Finished Maneuver detection\n");
}*/
