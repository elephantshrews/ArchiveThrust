#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define Window_size 4
#define sigThresh 2
// You can also declare other function prototypes used in main.c here if necessary
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


double (*listOfVelocities(const tle_storage tle_st))[3];
void detectManeuvers(const tle_storage tle_st,int window_size, double Sigthresh);
tle_storage tle_download_and_parse(void);

#endif // MAIN_H