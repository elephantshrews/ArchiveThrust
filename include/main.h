/*                                                                          
* ArchiveThrust - main.h
*
* Authors: Michiel Snoeken & Freddy Spaulding
* Created in 2024
* GNU General Public License
*
* 
*/
#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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


void detectManeuvers(const tlePermanentStorage *tleSt);
tlePermanentStorage tle_download_and_parse(void);

#endif // MAIN_H
