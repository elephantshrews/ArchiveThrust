/* 
 *
 * Archive Thrust - tle_download_and_parse.c
 *
 * Authors: Michiel Snoeken & Freddy Spaulding
 * GNU General Public License
 *
 *
*/

/* Dependencies */
#include "tle_download_and_parse.h"

#define KEY_BUFFER_SIZE 400 
#define LOGIN_URL "https://www.space-track.org/ajaxauth/login"
#define EMPTY_KEY_FORMAT "identity=%s&password=%s"


int login(char* username, char* password)
{
    /*
     * Function description:
     *
     * Login into spacetrack.org
     * and save the cookies
     * in cookies.txt so that
     * the function download
     * can accesse these 
     * and download
     * 
    */

        // Construct the key for login
    	char key[KEY_BUFFER_SIZE];
    	snprintf(key, sizeof(key), EMPTY_KEY_FORMAT, username, password);

        char *login_url = "https://www.space-track.org/ajaxauth/login";

        // Initialize libCurl 
        if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
            fprintf(stderr, "Global Initialization fails\n");
            return -1;
        }
        
	    CURL* curl = curl_easy_init();
        if (!curl) {
            fprintf(stderr, "cURL Initialization fails\n");
            cleanup_curl(curl);
            return -1;
        }

        // Response buffer to store server's reply
        char response[4096] = {0};

        // Set options for libcurl and perform PUSH request
	    curl_easy_setopt(curl, CURLOPT_URL, login_url); 
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, key);
	    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
       	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");


        // set callback to capture servers reponse 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_l);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

       // Perform the request
	    CURLcode res = curl_easy_perform(curl);
	  
        // Check if request was successful 
        if (res != CURLE_OK) { 
            fprintf(stderr, "%s \n", curl_easy_strerror(res));
            cleanup_curl(curl);
            return -1;
    	}	

        cleanup_curl(curl);
    	return 0; // Succesfull login
}

int download(char* norad_id, void* tlestor_init)
{
    /*
     * Function description:
     *
     * Download the TLES from
     * spacetrack.org from the satellite
     * with norad id as user input. It 
     * stores the TLEs in the heap
    */

        // Convert void pointer to tle pointer.
        // void pointer is only used to make it easier
        // for Python.
        TleStor* tlestor = (TleStor *) tlestor_init;
  
        // Create temporary storage for TLEs
        TleTemp tletemp;
        tletemp.str = malloc(1);
        tletemp.size = 0;

    	/* Download Data from Space-Track */ 
    	// Formulate GET request for Space-Track 
        char *start = "2020-01-01";

        // Get the current time
        time_t now = time(NULL);
        struct tm *tm_now = localtime(&now);
        // Create a st
        char end[11]; 
        // Format the date into the string
        strftime(end, sizeof(end), "%Y-%m-%d", tm_now);

    	
    	static const char *raw_url =	
        "https://www.space-track.org/basicspacedata/query/class/gp_history/NORAD_CAT_ID/%s/orderby/TLE_LINE1%%20ASC/EPOCH/%s--%s/format/tle";
        char url[400];
    	snprintf(url, sizeof(url), raw_url, norad_id, start, end);
	
        /* Initialize libCurl */
        curl_global_init(CURL_GLOBAL_ALL);
	    CURL* curl = curl_easy_init();
	    CURLcode res;
        if(!curl){
	    	fprintf(stderr, "Initialization fails\n");
	    	return -1;
	    }
	    curl_easy_setopt(curl, CURLOPT_URL, url);
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &tletemp);
	    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
       	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
	    printf("Downloading data . . .\n");
	    res = curl_easy_perform(curl);

	    if (res != CURLE_OK){
	    	fprintf(stderr, "%s/n", curl_easy_strerror(res));
	    }

        /* Clean up */
    	curl_easy_cleanup(curl);
    	curl_global_cleanup();
        // String gets disassembled into the indivual TLEs.
        // These invidual TLEs get stored in struct and all of
        // them together are stored in the list of TLEs
        // of tlePermanantStorage
        
        int tlenumber = lines_count(tletemp.str) / 2;
	    tlestor->nmemb = tlenumber; 
	    tlestor->tles = (TLE *) malloc(tlenumber * sizeof(TLE));
	    tle_parse(&tletemp, tlestor); 



        if (tlenumber == 0) {
            fprintf(stderr, "Error: Please check login credentials and NORAD ID. \n");
            return -1;
        }
        // Cleanup
        free(tletemp.str);
        tletemp.str = NULL;    


        // Remove the cookie file
        remove("cookies.txt");

    	return 0;


}


void* create_permanent_storage(void)
{
    /*
     * Function description:
     *
     * Initalize storage of TLEs
     * and return the pointer to this storage
     * as void pointer. This is done so that it
     * is easier to deal with in Python. 
    */
        // Allocate memory on the heap
        TleStor* tlestor_init = malloc(sizeof(TleStor));
        // Check if memory allocation was successful
        if (tlestor_init == NULL){
            fprintf(stderr, "Memory allocation failed\n");
            return NULL; // Return NULL to indicate failure
        }
       
        tlestor_init->tles = NULL;
        tlestor_init->nmemb = 0;

        return (void *) tlestor_init;
}

static size_t write(void *cont, size_t size, size_t nmemb, void *userp)
{
        // Get pointer to the temporary storage           
    	TleTemp* tletemp = (TleTemp *)userp;
    
        size_t strsize = size * nmemb;
    	char *ptr = realloc(tletemp->str, tletemp->size + strsize + 1);
    	if (ptr == NULL) {
    		printf("not enough memory (realloc returned NULL)\n");
    		return 0;
    	}

    	tletemp->str = ptr;
        memcpy(&(tletemp->str[tletemp->size]), cont, strsize);
        tletemp->size += strsize;
	    tletemp->str[tletemp->size] = 0; 
	    return strsize;
}

static size_t write_l(void* cont, size_t size, size_t nmemb, void* userp){
        size_t total_size = size * nmemb;
        strncat(userp, cont, total_size); // Append to userp buffer
        return total_size;
}

static void tle_line_one_parse(const char *line, tleLineOne *tle1) 
{ 
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

static void tle_line_two_parse(const char *line, tleLineTwo *tle2) 
{
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


static int tle_parse(TleTemp *tletemp, TleStor* tlestor){
	
	    // Open a memory stream for reading
	    FILE *stream = fmemopen(tletemp->str, tletemp->size, "r");
	    if (stream == NULL) {
	    	perror("fmemopen");
	    	return 1;
	    }

	
	    int count = 0;
	    int tle_nmb = 0;
	    char line[100];

	    while (fgets(line, sizeof(line), stream) != NULL) {
	    	line[strcspn(line, "\n")] = '\0';

	    	if (count % 2 == 0){
	    		tle_line_one_parse(line, &(tlestor->tles[tle_nmb].line1));
	    	}
	    	else if (count % 2 == 1){
	    		tle_line_two_parse(line, &(tlestor->tles[tle_nmb].line2));
	    		tle_nmb ++;		
	    	}
    
    		count++;
    	}

    	// Close memory stream
    	fclose(stream);
    	return 0;
}

static void cleanup_curl(CURL* curl)
{
        curl_easy_cleanup(curl);
        curl_global_cleanup();
}


static int lines_count(char *str)
{
	    int lines = 0;
	    for (int i = 0; str[i]; i++) {
	    	if (str[i] == '\n') lines++;
    	}
    	return lines;
}
