#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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
} tleLineOne;

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
} tleLineTwo;

typedef struct {
	tleLineOne line1;
	tleLineTwo line2;
} TLE;

typedef struct {
    char *str;
    size_t size;
} tleTemporaryStorage;


typedef struct {
    TLE *tles;
    size_t nmemb;
} tlePermanentStorage;


double (*listOfVelocities(const tlePermanentStorage tle_st))[3];
//void detectManeuvers(const tle_storage *tle_st);
void multOrbParams(const tlePermanentStorage *tleSt);
tlePermanentStorage tle_download_and_parse(void);

#endif // MAIN_H