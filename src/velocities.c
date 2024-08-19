#include "velocities.h"
#include <math.h>
#include "utils.h"
void computeVelocityFromTLE(const TLE_Line2 *tle2, double *v_x, double *v_y, double *v_z) {
    // Step 1: Convert mean motion to semi-major axis (mean motion is in revs per day)
    double mean_anomaly = tle2->meanAnomaly;
    double ecc = tle2->eccentricity;
    double inc = tle2->inclination;
    double raan = tle2->raan;
    double arg_perigee = tle2->argPerigee;
    double mean_motion = tle2->meanMotion;
    double n = mean_motion * 2 * 3.14159 / 86400.0; // Convert to rad/s
    double a = pow(MU / (n * n), 1.0 / 3.0);  // Semi-major axis in km
    // Step 2: Solve Kepler's equation to get the eccentric anomaly E
    double E = mean_anomaly;  // Initial guess (could iterate for better accuracy)
    double delta = 1e-6;
    while (fabs(mean_anomaly - (E - ecc * sin(E))) > delta) {
        E = E - (E - ecc * sin(E) - mean_anomaly) / (1 - ecc * cos(E));
    }
    
    // Step 3: Compute the true anomaly
    double true_anomaly = 2 * atan2(sqrt(1 + ecc) * sin(E / 2), sqrt(1 - ecc) * cos(E / 2));
    
    // Step 4: Compute position and velocity in orbital plane
    double r = a * (1 - ecc * cos(E)); // Distance
    double p = a * (1 - ecc * ecc);    // Semi-latus rectum
    double v_r = sqrt(MU / p) * ecc * sin(true_anomaly);  // Radial velocity component
    double v_t = sqrt(MU / p) * (1 + ecc * cos(true_anomaly));  // Tangential velocity component

    // Position in the orbital plane
    double r_x = r * cos(true_anomaly);
    double r_y = r * sin(true_anomaly);

    // Step 5: Convert to ECI coordinates using orbital parameters
    // Simplified rotation (for demonstration purposes; need full rotation matrix in actual implementation)
    *v_x = v_r * cos(raan) - v_t * sin(raan) * cos(inc);
    *v_y = v_r * sin(raan) + v_t * cos(raan) * cos(inc);
    *v_z = v_t * sin(inc);
    //printf("this should be a velocity comp.: %f\n ", *v_z);
}
void computeDeltaV(double vx1, double vy1, double vz1, double vx2, double vy2, double vz2, double *delvx, double *delvy, double *delvz) {
     *delvx = vx2 - vx1 ;
     *delvy = vy2 - vy1 ;
     *delvz = vz2 - vz1 ;
        
}
double computeVectorMagnitude(double x, double y, double z) {
    double mag = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
    return mag;
}

double (*listOfVelocities(const tle_storage tle_st))[3] {
    size_t nmemb = tle_st.nmemb;
    printf("Number of tles: %ld\n", nmemb);
    // Dynamically allocate memory for the velocities
    double (*velocities)[3] = (double (*)[3])malloc(nmemb * sizeof(*velocities));
    printf("Finished allocating memory\n");
    if (velocities == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);  // Exit if allocation fails
    }
    printf("Memory allocation successfull\n");
    int count = 0;
    printf("Starting computation of velocities...\n");
    while (count < nmemb) {
        computeVelocityFromTLE(&tle_st.tles[count].line2, &velocities[count][0], &velocities[count][1], &velocities[count][2]);
        count++;
    }
    printf("Finished calculation of velocities\n");

    return velocities;
}


