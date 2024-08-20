/*
 * Author: Michiel Snoeken & Freddy Spaulding
 * Purpose: Archive Thrust
 * Copyright 2024
*/

#include "main.h"
#include "maneuvers.h"
#include "tle_download_and_parse.h"

// Dependencies 


int main() {    
    //signal(SIGINT, handle_sigint);

    int windowSize = Window_size;
    double sigmaThreshold = sigThresh;
    
    // Assuming you have a working tle_download_and_parse function
    printf("Starting tle download and parsing ...\n");

    tle_storage tle_stor = tle_download_and_parse();
    
    printf("Finished tle download and parsing \n");
    // Dynamically allocate memory for velocities

    printf("Starting calculation of velocities... \n");
    double (*velocities)[3] = (double (*)[3])listOfVelocities(tle_stor);
    printf("Finished calculating velocities \n");
    printf("Starting maneuver detection\n");
    printf("Number of members; not erased: %ld\n", tle_stor.nmemb);
    detectManeuvers(velocities, tle_stor.nmemb, windowSize, sigmaThreshold);
    printf("Finished Maneuver detection\n");
    // Example output of the first velocity component
    printf("First velocity component: %f\n", velocities[0][0]);

    // Free the dynamically allocated memory after use
    free(velocities);

    return 0;
}

