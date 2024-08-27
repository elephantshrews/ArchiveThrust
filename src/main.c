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

    tle_storage tle_stor = tle_download_and_parse();
    
    detectManeuvers(&tle_stor);

    return 0;
}

