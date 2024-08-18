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


int main(void) {

// Store the TLES in the heap
tle_storage tle_stor;
tle_download_and_parse(tle_stor);



    return 0;
}
