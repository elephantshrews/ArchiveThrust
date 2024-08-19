/*
 * Author: Michiel Snoeken & Freddy Spaulding
 * Purpose: Archive Thrust
 * Copyright 2024
*/

#include "main.h"

// Dependencies 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tle_download_and_parse.h"
#include "velocities.h"
#include "maneuvers.h"
#include "utils.h"


int main() {    
    //signal(SIGINT, handle_sigint);

    int windowSize = Window_size;
    double sigmaThreshold = sigThresh;
    
    // Assuming you have a working tle_download_and_parse function
    printf("Starting tle download and parsing ...\n");

    tle_storage tle_stored = tle_download_and_parse();
    
    printf("Finished tle download and parsing \n");
    // Dynamically allocate memory for velocities

    printf("Starting calculation of velocities... \n");
    double (*velocities)[3] = (double (*)[3])listOfVelocities(tle_stored);
    printf("Finished calculating velocities \n");
    printf("Starting maneuver detection\n");
    printf("Number of members; not erased: %ld\n", tle_stored.nmemb);
    detectManeuvers(velocities, tle_stored.nmemb, windowSize, sigmaThreshold);
    printf("Finished Maneuver detection\n");
    // Example output of the first velocity component
    printf("First velocity component: %f\n", velocities[0][0]);

    // Free the dynamically allocated memory after use
    free(velocities);

    return 0;
}

