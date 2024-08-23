/*
 * Author: Michiel Snoeken & Freddy Spaulding
 * Purpose: Archive Thrust
 * Copyright 2024
*/

#include "main.h"
#include "detect_maneuvers.h"
#include "tle_download_and_parse.h"

// Dependencies 


int main() {    
    //signal(SIGINT, handle_sigint);

    int windowSize = Window_size;
    double sigmaThreshold = sigThresh;
    
    // Assuming you have a working tle_download_and_parse function

    tle_storage tle_stor = tle_download_and_parse();
    
    // Dynamically allocate memory for velocities

    //double (*velocities)[3] = (double (*)[3])listOfVelocities(tle_stor);
    
    detectManeuvers(tle_stor, windowSize, sigmaThreshold);

    
    // Example output of the first velocity component

    // Free the dynamically allocated memory after use
    //free(&tle_stor);

    return 0;
}

