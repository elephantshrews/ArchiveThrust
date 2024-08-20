#ifndef MANEUVERS_H
#define MANEUVERS_H
#include <stddef.h> // For size_t
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#define Window_size 5
#define sigThresh 3
// Constants
#define MU 398600.4418 // Gravitational parameter for Earth in km^3/s^2


// Function prototypes
double findMedian(double *arr, int size);
void bubbleSort(double *arr, int size);
void computeVelocityFromTLE(const TLE_Line2 *tle2, double *v_x, double *v_y, double *v_z);
void computeDeltaV(double vx1, double vy1, double vz1, double vx2, double vy2, double vz2, double *delvx, double *delvy, double *delvz);
double computeVectorMagnitude(double x, double y, double z);
double (*listOfVelocities(const tle_storage tle_st))[3];



void detectManeuvers(double velocities[][3], int dataSize, int window_size, double Sigthresh);

#endif // MANEUVERS_H