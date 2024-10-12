
/*                                                                          
* ArchiveThrust - detect_maneuvers.h
*
* Authors: Michiel Snoeken & Freddy Spaulding
* Created in 2024
* GNU General Public License
*
* 
*/
#ifndef MANEUVERS_H
#define MANEUVERS_H

//Including the standard libraries and libraries necessary for polyomial fitting
#include <stddef.h> // For size_t
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "main.h"

//Setting the parameters of the Fading Window Polynomial Fitting
#define MIN_WINDOW_SIZE 4
#define MAX_WINDOW_SIZE 15
#define INITIAL_WINDOW_SIZE 7
#define WindowSize 7
//#define sigmaThreshold   15 //15
#define MAX_POLY_DEGREE 2
#define POLY_DEGREE 1
#define FADE_FACTOR 0.9



//Maneuver type
#define IN_PLANE 0
#define OUT_OF_PLANE 1
#define ORBIT_RAISING_LOWERING 2
#define PLANE_CHANGE 3
#define STATION_KEEPING 4
#define PERIGEE_APOGEE_CHANGE 5
#define PHASING 6
#define DRAG_COMPENSATION 7

// Function prototypes


void _extractOrbParams(const TleStor *tle_st, double *epochYears, double *epochDays, double *meanMotions, double *inclinations, double *eccentricities, double *argPerigee, double *raan, double *meanAnomaly);
double _findAvFluct(const double *data, const double *fittedData, int realSizeOfWindow);
int _selectBestPolynomialDegree(const double *epochDays, const double *orbitParams, int windowSize);
void _fitFadingMemoryPolynomial(const double *epochDays, const double *orbitParams, int windowSize, double *coefficients, int poly_degree);
//void _fitFadingMemoryPolynomial(const double *epochDays, const double *orbitParams, int windowSize, double *coefficients);
bool isCloseEnough(double epochDay1, double epochDay2, int threshold);
void classifyManeuver(Maneuver *maneuver);
void _singleParamDetection(const TleStor *tleSt, int nmemb, Maneuver *detectedManeuvers);
void _calculateConfidenceLevel(double *ripples, int maneuverType, int numbOrbitParams, double *deviations);
void detectManeuvers(const TleStor *tleSt, Maneuver *detectedManeuvers);


#endif // MANEUVERS_H