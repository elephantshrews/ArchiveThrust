#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

// Credentials for Space-Track
#define USERNAME "m.snoeken@campus.tu-berlin.de"
#define PASSWORD "UfQx95rK5Bj4haRhiKBP"
//define constants/parameters
#define MAX_TLE_ENTRIES  50
#define start_year 93
#define end_year 96 
#define EARTH_RADIUS 6378.137 // km
#define MU 398600.4418 // km^3/s^2
//#define POLY_DEGREE 1// Degree of the polynomial
//#define FADE_FACTOR 1 // Fading factor (close to 1 means slow fading)
#define Window_size 5
#define sigThresh 3


// Datastructure that containts the first line of a TLE
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



// Before processing
typedef struct {
	char *str;
	size_t size;
} temp_storage;


// After processing
// Pointer that points to first TLE (in array of TLES).
typedef struct {
	TLE *tles;
	size_t nmemb;
} tle_storage;

int count_lines(char *str);

// Keep in mind that this function gets called more than once
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
    // Compute size of URL.
    size_t realsize = size * nmemb;
    // fprintf(stderr, "Got %ld bytes \n \n", realsize);
	
	temp_storage* temp_stor = (temp_storage *)userp;
    


    // Reallocate memory to fit the new data
	char *ptr = realloc(temp_stor->str, temp_stor->size + realsize + 1);
	if (ptr == NULL) {
		// out of memory
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

    // Copy the new data into the memory block
	temp_stor->str = ptr;
    memcpy(&(temp_stor->str[temp_stor->size]), contents, realsize);
    temp_stor->size += realsize;
	temp_stor->str[temp_stor->size] = 0; 


	return realsize;
}

// Return pointer to memory
int tle_download(const char *username, const char *password, temp_storage* temp_stor){
	

	// Initialize libcurl. Not so clear what this  intilization means
	// but it is a bit like opening an application on your desktop.
	curl_global_init(CURL_GLOBAL_ALL);

	// curl is a pointer to the CURL struct, which
	// contains all the information on the state 
    // of our transfer. It's like the control room
	// of our action and we need to tell the control
	// room how to do certain things.

	// Data to login.
	char login[200];
	char *credentials = "identity=%s&password=%s";
	snprintf(login, sizeof(login), credentials,  username, password);

	char *login_url =
	"https://www.space-track.org/ajaxauth/login";

	char url[200];
	int NORAD_ID;
	printf("Please enter NORAD ID: ");
	scanf("%d", &NORAD_ID);

	const char *start_time = "1950-01-01";
	const char *end_time = "2024-08-04";
	static const char *base_url =
	"https://www.space-track.org/basicspacedata/query/class/gp_history/NORAD_CAT_ID/%d/orderby/TLE_LINE1%%20ASC/EPOCH/%s--%s/format/tle";


	// Construct the final TLE
	snprintf(url, sizeof(url), base_url, NORAD_ID, start_time, end_time);
	
	CURL* curl = curl_easy_init();
	CURLcode res;
	
	// Check if everything went correctly after initialization
	if(!curl){
		fprintf(stderr, "Initialization fails\n");
		return -1;
	}	

	// Now we tell the control room how to do our action.

	// This option tells the control room what to do with
	// the data that we extract. We tell the control room
	// to take our function write_data as a manual for storing
	// our data.
	
	// Specify URL to extract data from.
	curl_easy_setopt(curl, CURLOPT_URL, login_url); 
	
	// Specify how to process extracted data.
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	res = curl_easy_perform(curl);
	if (res != CURLE_OK){
		fprintf(stderr, "curl_easy_perform() return %s\n",
						curl_easy_strerror(res));
	}	
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) temp_stor);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK){
		fprintf(stderr, "curl_easy_perform() return %s/n",
									curl_easy_strerror(res));
	}


	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return 0;
}


// These two functions take a line as an input and store it correctly in the TLE_datastructures.
void parseTLELine1(const char *line, TLE_Line1 *tle1) { 
   char launchPiece[4] = {0};
    sscanf(line, "%1d%5d%1c%2d%3d%3s%2d%12f%10f%8f%9f%1d%4d%1d",
           &tle1->lineNumber,
           &tle1->satelliteNumber,
           &tle1->classification,
           &tle1->launchYear,
           &tle1->launchNumber,
           launchPiece,
           &tle1->epochYear,
           &tle1->epochDay,
           &tle1->meanMotionDerivative,
           &tle1->meanMotionSecondDer,
           &tle1->bstar,
           &tle1->ephemerisType,
           &tle1->elementSetNumber,
           &tle1->checksum);
    strcpy(tle1->launchPiece, launchPiece);
}

void parseTLELine2(const char *line, TLE_Line2 *tle2) {
    int eccentricityRaw;
    sscanf(line, "%1d%5d%9f%9f%7d%9f%9f%9f%5d%1d",
           &tle2->lineNumber,
           &tle2->satelliteNumber,
           &tle2->inclination,
           &tle2->raan,
           &eccentricityRaw,
           &tle2->argPerigee,
           &tle2->meanAnomaly,
           &tle2->meanMotion,
           &tle2->revNumber,
           &tle2->checksum);
    tle2->eccentricity = (float)eccentricityRaw / 10000000.0f; 
}

void tle_print(TLE *tle){
	printf("MeanMotionDerivative is: %f \n", tle->line1.meanMotionDerivative);
	printf("MeanMotionSecondDer is: %f \n", tle->line1.meanMotionSecondDer);
	printf("bstar is: %f \n", tle->line1.bstar);
	printf("ephemerisType is: %d \n", tle->line1.ephemerisType);
	printf("elementSetNumber is: %d \n", tle->line1.elementSetNumber);

}


int tle_parse(temp_storage *temp_stor, tle_storage* tle_stor){
	
	int count = 0;
	int tle_nmb = 0;
	char line[100];

	// Open a memory stream for reading
	FILE *stream = fmemopen(temp_stor->str, temp_stor->size, "r");
	if (stream == NULL) {
		perror("fmemopen");
		return 1;
	}

	
	while (fgets(line, sizeof(line), stream) != NULL) {
		line[strcspn(line, "\n")] = '\0';

		if (count % 2 == 0){
			parseTLELine1(line, &(tle_stor->tles[tle_nmb].line1));
		}
		else if (count % 2 == 1){
			parseTLELine2(line, &(tle_stor->tles[tle_nmb].line2));
			tle_nmb ++;		
		}

		count++;
	}


	// Close memory stream
	fclose(stream);

	return 0;
}
		//computing the orbital parameters

typedef struct {
    double epochDay;
    int epochYear;
    double meanMotion;
    double semiMajorAxis;
    double eccentricity;
    double orbitalEnergy;
} in_plane_params;

typedef struct {
    double epochDay;
    int epochYear;
    double inclination;
    double node;
}out_of_plane_params;

in_plane_params compute_in_plane_parameters(const TLE_Line1 *tle1, const TLE_Line2 *tle2) {
    in_plane_params params;
    double n = tle2->meanMotion; // * 2.0 * 2*acos(0)/ 86400.0; // Convert revs per day to rad/s
    params.epochYear = tle1->epochYear;
    params.epochDay = tle1->epochDay;// + (params.epochYear-start_year)*365;
    params.meanMotion = tle2->meanMotion;
    params.semiMajorAxis = pow(MU / (n * n), 1.0 / 3.0);
    params.eccentricity = tle2->eccentricity;
    params.orbitalEnergy = -MU / (2.0 * params.semiMajorAxis);
    // Compute other parameters as needed

    return params;
}
out_of_plane_params computeout_of_plane_parameters(const TLE_Line1 *tle1, const TLE_Line2 *tle2){
    out_of_plane_params params;
    params.epochYear = tle1->epochYear;
    params.epochDay = tle1->epochDay; // + (params.epochYear-start_year)*365;
    params.inclination = tle2->inclination;
    params.node = tle2->raan;
    return params;
};

// main
tle_storage tle_download_and_parse(){
	// Temporarily store the data in the stack
	temp_storage temp_stor;
	temp_stor.str = malloc(1); 
	temp_stor.size = 0;


	// This function stores a string with the
	// tles in the stack.
	tle_download(USERNAME, PASSWORD, &temp_stor);

		
	size_t nmemb = count_lines(temp_stor.str) / 2;
	// Now we want to permamently save the parsed 
	// TLES in the heap

	// Declare tle_storage
	tle_storage tle_stor;

	// Initialize the storage to contain nmemb TLEs.
	tle_stor.nmemb = nmemb;
	tle_stor.tles = (TLE *) malloc(nmemb * sizeof(TLE));
	tle_parse(&temp_stor, &tle_stor); 
    free(temp_stor.str);
    temp_stor.str = NULL;    
	printf("Number of TLEs: %ld \n", nmemb);
	return tle_stor;
}
// Function 
int count_lines(char *str){
	int lines = 0;
	for (int i = 0; str[i]; i++){
		if (str[i] == '\n') lines++;
	}
	return lines;
}

                    //START OF DATA ANALYSIS
// Function to compute velocity vector from TLE
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

/*
double* listOfVelocities(const tle_storage tle_st) {
    size_t nmemb = tle_st.nmemb;
    double velocities[nmemb][3];
    int count = 0;
    while (count< nmemb) {
        computeVelocityFromTLE(&tle_st.tles[count].line2,&velocities[count][0], &velocities[count][1], &velocities[count][2]);
        count++;
    }
    return velocities;
}
*/
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
    printf("Freeing up storage of tles\n");
    if (tle_st.tles != NULL) {
        free(tle_st.tles);  // Free the dynamically allocated array of TLEs
        //tle_st->tles = NULL;  // Optional: Set to NULL to avoid dangling pointer
    }
    printf("Storage is now free\n");
    return velocities;
}

void bubbleSort(double *arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap arr[j] and arr[j + 1]
                double temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
// Function to find the median of the array
double findMedian(double *arr, int size) {
    // Create a copy of the array to sort
    double *sortedArr = (double *)malloc(size * sizeof(double));
    memcpy(sortedArr, arr, size * sizeof(double));
    
    // Sort the array using Bubble Sort
    bubbleSort(sortedArr, size);
    
    // Calculate the median
    double median;
    if (size % 2 == 0) {
        median = (sortedArr[size / 2 - 1] + sortedArr[size / 2]) / 2.0;
    } else {
        median = sortedArr[size / 2];
    }
    
    // Free the allocated memory
    free(sortedArr);
    //printf("this is the median: \n%f\n\n", median);
    return median;
}

// Function to detect maneuvers using the median filter method
void detectManeuvers(double velocities[][3], int dataSize,int window_size, double Sigthresh) {
    printf("initializing detection\n");
    double deltaV[dataSize - 1][3];  // Array to store delta V vectors
    double deltaVMagnitudes[dataSize - 1];  // Array to store magnitudes of delta V vectors

    // Compute delta V vectors and their magnitudes
    printf("computing Delta Vs\n");
    for (int i = 0; i < dataSize - 1; i++) {
        printf("count: %d\n", i);
        computeDeltaV(velocities[i][0], velocities[i][1], velocities[i][2],
                      velocities[i + 1][0], velocities[i + 1][1], velocities[i + 1][2],
                      &deltaV[i][0], &deltaV[i][1], &deltaV[i][2]);
        //printf("this should be a double: %f\n", deltaV[i][0]);
        deltaVMagnitudes[i] = computeVectorMagnitude(deltaV[i][0], deltaV[i][1], deltaV[i][2]);
        
        
    }

    // Apply median filter and detect maneuvers
    for (int i = window_size; i < dataSize - 1; i++) {
        // Create a window of delta V magnitudes
        double window[window_size];
        for (int j = 0; j < window_size; j++) {
            window[j] = deltaVMagnitudes[i - j];
        }
        // Compute the median of the window
        double median = findMedian(window, window_size);
        
        // Check if the current delta V magnitude exceeds the threshold relative to the median
        double deviation = fabs(deltaVMagnitudes[i] - median);
        //printf("This is the deviation: %f\n", deviation);
        if (deviation > Sigthresh) {
            printf("Potential maneuver detected at index %d (Epoch: %d)\n", i + 1, i + 1);
            printf("Delta V magnitude: %f, Median: %f, Deviation: %f\n", deltaVMagnitudes[i], median, deviation);
        }
    }
}
/*
void handle_sigint(int sig) {
    printf("Caught signal %d. Cleaning up and exiting...\n", sig);

    // Free the dynamically allocated memory
    if (velocities != NULL) {
        free(velocities);
        velocities = NULL;  // Optional, but good practice to avoid dangling pointers
    }

    if (tle_stored.tles != NULL) {
        free(tle_stored.tles);
        tle_stored.tles = NULL;  // Optional, but good practice to avoid dangling pointers
    }

    // Exit the program
    exit(0);
}
*/
int main() {    
    //signal(SIGINT, handle_sigint);

    int windowSize = Window_size;
    double sigmaThreshold = sigThresh;
    // Assuming you have a working tle_download_and_parse function
    printf("Starting tle download and parsing ...\n");

    tle_storage tle_stored = tle_download_and_parse();
    
    printf("Finished tle download and parsing \n");
    // Dynamically allocate memory for velocities

    printf("Starting calculation of velocities... \n");
    double (*velocities)[3] = (double (*)[3])listOfVelocities(tle_stored);
    printf("Finished calculating velocities \n");
    printf("Starting maneuver detection\n");
    printf("Number of members; not erased: %ld\n", tle_stored.nmemb);
    detectManeuvers(velocities, tle_stored.nmemb, windowSize, sigmaThreshold);
    printf("Finished Maneuver detection\n");
    // Example output of the first velocity component
    printf("First velocity component: %f\n", velocities[0][0]);

    // Free the dynamically allocated memory after use
    free(velocities);

    return 0;
}
/*
int main() {
    double velocities[MAX_TLE_ENTRIES][3];
    TLE_Line1 tle1[MAX_TLE_ENTRIES];
    TLE_Line2 tle2[MAX_TLE_ENTRIES];
    TLE tle_entries[MAX_TLE_ENTRIES];
    in_plane_params params_in[MAX_TLE_ENTRIES];
    out_of_plane_params params_out[MAX_TLE_ENTRIES];
    FILE *file = fopen("tle_data_filtered.txt", "r");
    if (!file) {
        perror("Unable to open file");
        return EXIT_FAILURE;
    }
    int count = 0;
    while (count < MAX_TLE_ENTRIES && fgets(tle_entries[count].line1,sizeof(tle_entries[count].line1),file)) {
        if (fgets(tle_entries[count].empty,sizeof(tle_entries[count].empty),file)==NULL) break;
        if (fgets(tle_entries[count].line2, sizeof(tle_entries[count].line2), file) == NULL) break;
        if (fgets(tle_entries[count].empty2,sizeof(tle_entries[count].empty2),file)==NULL) break;
        tle_entries[count].line1[strcspn(tle_entries[count].line1, "\n")+1] = '\0';
        tle_entries[count].line2[strcspn(tle_entries[count].line2, "\n")+1] = '\0';
        
        parseTLELine1(tle_entries[count].line1, &tle1[count]);
        parseTLELine2(tle_entries[count].line2, &tle2[count]);

        params_in[count] = compute_in_plane_parameters(&tle1[count], &tle2[count]);
        params_out[count] = computeout_of_plane_parameters(&tle1[count],&tle2[count]);
        printf("this should be 0-1: %f\n",params_in[count].eccentricity);
        computeVelocityFromTLE(&tle2[count], &velocities[count][0], &velocities[count][1], &velocities[count][2] );
        
        //printf("epoch day: %f\n", params_in[count].epochDay);
        count++;
    }
    int dataSize = MAX_TLE_ENTRIES;
    int windowSize = Window_size;
    double sigmaThreshold = sigThresh;
    detectManeuvers(velocities, dataSize, windowSize,  sigmaThreshold);
    printf("2\n");
    return 0;

    }
*/
/*
int main() {
    // Call the `tle_download_and_parse` function
    tle_download_and_parse();
    return 0;
}*/