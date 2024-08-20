#include "tle_download_and_parse.h"
// Credentials for Space-Track
#define USERNAME "m.snoeken@campus.tu-berlin.de"
#define PASSWORD "UfQx95rK5Bj4haRhiKBP"


// Keep in mind that this function gets called more than once
int count_lines(char *str){
	int lines = 0;
	for (int i = 0; str[i]; i++){
		if (str[i] == '\n') lines++;
	}
	return lines;
}

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
int tle_download(const char *usrname, const char *password, temp_storage* temp_stor){
	

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
	snprintf(login, sizeof(login), credentials,  usrname, password);

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


tle_storage tle_download_and_parse(){
	// Temporarily store the data in the stack
	temp_storage temp_stor;
	temp_stor.str = malloc(1); 
	temp_stor.size = 0;


	// This function stores a string with the
	// tles in the stack.
	printf("Starting tle download and parsing ...\n");

	tle_download(USERNAME, PASSWORD, &temp_stor);
	
	printf("Finished tle download and parsing \n");
		
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
