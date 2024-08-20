#ifndef  STRUCTURES_H
#include "gsl/gsl_matrix.h"
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
//#include <curl/curl.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <math.h>

typedef struct {
    int     lineNumber;
    int     satelliteNumber;
    char    classification;
    int     launchYear;
    int     launchNumber;
    char    launchPiece[4];
    int     epochYear;
    float   epochDay;
    float meanMotionDerivative;
    float meanMotionSecondDer;
    float bstar;
    int ephemerisType;
    int elementSetNumber;
    int checksum;
} TLE_Line1;

// Datastructure that contains the second line of a TLE
typedef struct {
    int lineNumber;
    int satelliteNumber;
    float inclination;
    float raan;
    float eccentricity;
    float argPerigee;
    float meanAnomaly;
    float meanMotion;
    int revNumber;
    int checksum;
} TLE_Line2;

typedef struct {
	TLE_Line1 line1;
	TLE_Line2 line2;
} TLE;

typedef struct {
    char *str;
    size_t size;
} temp_storage;

typedef struct {
    TLE *tles;
    size_t nmemb;
} tle_storage;


#endif // STRUCTURES_H