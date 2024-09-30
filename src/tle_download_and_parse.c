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


TleStor* tleDownloadParse() 
{
        /* 
         * Function Description:
         *
         * Download TLE Archive from Space-Track.org
         * and store the parsed data in the data struct
         * TleStor, which contains a list of
         * tles, which is yet another struct contaning 
         * all the orbital elements and the number of tles. These structs
         * are defined in main.h
        */



        // URL content gets temporarily stored as a string
        char* username = "m.snoeken@campus.tu-berlin.de";
        char* password = "UfQx95rK5Bj4haRhiKBP";
        char* norad_id = "5";

        TleTemp* tletemp = create_temporary_storage();
        TleStor* tlestor = create_permanent_storage();
        login(username, password);
	    download(norad_id, tletemp, tlestor);
		
	    return tlestor;
}

int login(char* username, char* password)
{

        char *emptykey = "identity=%s&password=%s"; 
    	char key[250];
    	snprintf(key, sizeof(key), emptykey,  username, password);

        /* Initialize libCurl */
        curl_global_init(CURL_GLOBAL_ALL);
	    CURL* curl = curl_easy_init();
	    CURLcode res;
	   
       if(!curl){
	    	fprintf(stderr, "Initialization fails\n");
	    	return -1;
	    }
        // Set options for libcurl and perform PUSH request
	    curl_easy_setopt(curl, CURLOPT_URL, "https://www.space-track.org/ajaxauth/login"); 
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, key);
	    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	    res = curl_easy_perform(curl);
	   
       if (res != CURLE_OK) { 
            fprintf(stderr, "%s \n", curl_easy_strerror(res));
    	}	

        /* Clean up */
    	curl_easy_cleanup(curl);
    	curl_global_cleanup();
    	return 0;
}

int download(char* norad_id, TleTemp* tletemp, TleStor* tlestor)
{
        
        tletemp->str = malloc(1);
        tletemp->size = 0;
        
    	/* Download Data from Space-Track */ 
    	// Formulate GET request for Space-Track 
        static const char *start = "2010-01-01";
    	static const char *end   = "2015-08-04";

    	
    	static const char *raw_url =	
        "https://www.space-track.org/basicspacedata/query/class/gp_history/NORAD_CAT_ID/%d/orderby/TLE_LINE1%%20ASC/EPOCH/%s--%s/format/tle";
        char url[200];
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
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) tletemp);
	    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	    printf("Starting tle download and parsing ...\n");
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
        
        int tlenumber = lines_count(tletemp->str) / 2;
	    tlestor->nmemb = tlenumber; 
	    tlestor->tles = (TLE *) malloc(tlenumber * sizeof(TLE));
	    tle_parse(tletemp, tlestor); 

        // Cleanup
        free(tletemp->str);
        tletemp->str = NULL;    

    	return 0;
}


TleTemp* create_temporary_storage(void)
{
        TleTemp* tletemp = malloc(sizeof(TleTemp));
        // Allocate memory on the heap
        if (tletemp == NULL){
            // Check if memory allocation was successful
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        return tletemp;
}

TleStor* create_permanent_storage(void)
{
        // Allocate memory on the heap
        TleStor* tlestor = malloc(sizeof(TleStor));
        // Check if memory allocation was successful
        if (tlestor == NULL){
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        return tlestor;
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

static int lines_count(char *str)
{
	    int lines = 0;
	    for (int i = 0; str[i]; i++) {
	    	if (str[i] == '\n') lines++;
    	}
    	return lines;
}
