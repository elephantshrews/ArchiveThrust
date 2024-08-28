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
#define WindowSize 10
#define sigmaThreshold 20
#define POLY_DEGREE 2
#define FADE_FACTOR 0.9
// Function prototypes


void _extractOrbParams(const tlePermanentStorage *tle_st, double *epochYears, double *epochDays, double *meanMotions, double *inclinations, double *eccentricities);
double _findAvFluct(const double *data, const double *fittedData, int realSizeOfWindow);
void _fitFadingMemoryPolynomial(const double *epochDays, const double *orbitParams, int windowSize, double *coefficients);
void _singleParamDetection(const double *orbitParams, const double *epochYears, const double *epochDays, int nmemb);
void detectManeuvers(const tlePermanentStorage *tleSt);

//FOR VALIDATION
#define MAX_LINE_LENGTH 512
#define MAX_REST_LENGTH 473

typedef struct {
    char line1[MAX_LINE_LENGTH];
} maneuverLine;

typedef struct {
    char satID[6]; // Changed to 6 to accommodate null terminator
    int yearbegin;
    int daybegin;
    int hourbegin;
    int minutebegin;
    int yearend;
    int dayend;
    int hourend;
    int minuteend;
    char rest[MAX_REST_LENGTH + 1]; // Added 1 for null terminator
} manLine;



#endif // MANEUVERS_H
