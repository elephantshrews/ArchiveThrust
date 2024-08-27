#ifndef MANEUVERS_H
#define MANEUVERS_H
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
// Constants
#define MU 398600.4418 // Gravitational parameter for Earth in km^3/s^2

#define WindowSize 7
#define sigmaThreshold 10
#define POLY_DEGREE 1
#define FADE_FACTOR 0.9
// Function prototypes

void extractOrbParams(const tlePermanentStorage *tle_st, double *epochYears, double *epochDays, double *meanMotions, double *inclinations, double *eccentricities);
bool IsInList(int index, int *list_of_indices, int indices_count);
double findAvFluct(const double *data, const double *fittedData, int realSizeOfWindow);
void detectManeuvers(const double *orbitParams, const double *epochYears, const double *epochDays, int nmemb);
void multOrbParams(const tlePermanentStorage *tleSt);

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

void parsemanLine(const char *line, manLine *manlineparsed);
manLine* parse_and_return();


#endif // MANEUVERS_H