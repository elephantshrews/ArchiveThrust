#ifndef VELOCITIES_H
#define VELOCITIES_H

#include <stddef.h> // For size_t
#include "velocities.h" // Assuming TLE_Line2 and tle_storage are defined here

// Constants
#define MU 398600.4418 // Gravitational parameter for Earth in km^3/s^2

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



// Function prototypes
void computeVelocityFromTLE(const TLE_Line2 *tle2, double *v_x, double *v_y, double *v_z);
void computeDeltaV(double vx1, double vy1, double vz1, double vx2, double vy2, double vz2, double *delvx, double *delvy, double *delvz);
double computeVectorMagnitude(double x, double y, double z);
double (*listOfVelocities(const tle_storage tle_st))[3];

#endif // VELOCITIES_H

