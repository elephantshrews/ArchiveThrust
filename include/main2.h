/*
 * Author: Michiel Snoeken & Freddy Spaulding
 * Purpose: Import TLEs from various sources
 * Copyright 2024
*/

#ifndef MYTPES_H
#define MYTPES_H

// Dependencies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void tle_download_and_parse();

// Datastructure that containts the first line of a TLE
typedef struct {
    int     lineNumber;
    int     satelliteNumber;
    char    classification;
    int     launchYear;
    int     launchNumber;
    char    launchPiece[4];
    int     epochYear;
    float   epochDay;
    float   meanMotionDerivative;
    float   meanMotionSecondDer;
    float   bstar;
    int     ephemerisType;
    int     elementSetNumber;
    int     checksum;
} TLE_Line1;

// Datastructure that contains the second line of a TLE
typedef struct {
    int     lineNumber;
    int     satelliteNumber;
    float   inclination;
    float   raan;
    float   eccentricity;
    float   argPerigee;
    float   meanAnomaly;
    float   meanMotion;
    int     revNumber;
    int     checksum;
} TLE_Line2;

typedef struct {
	TLE_Line1 line1;
	TLE_Line2 line2;
} TLE;



// Before processing
typedef struct {
	char    *str;
	size_t  size;
} temp_storage;


// After processing
// Pointer that points to first TLE (in array of TLES).
typedef struct {
	TLE     *tles;
	size_t  nmemb;
} tle_storage;



#endif



