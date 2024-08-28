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


tlePermanentStorage tleDownloadParse() 
{
        /* 
         * Function Description:
         *
         * Download TLE Archive from Space-Track.org
         * and store the parsed data in the data struct
         * tlePermanentStorage, which contains a list of
         * tles, which is yet another struct contaning 
         * all the orbital elements and the number of tles. These structs
         * are defined in main.h
        */



        // URL content gets temporarily stored as a string
    	tleTemporaryStorage tletemporarystorage;
	    tletemporarystorage.str = malloc(1); 
	    tletemporarystorage.size = 0;


	    _tleDownload(&tletemporarystorage);
	    printf("Finished tle download and parsing \n");
		
        // String gets disassembled into the indivual TLEs.
        // These invidual TLEs get stored in struct and all of
        // them together are stored in the list of TLEs
        // of tlePermanantStorage
	    tlePermanentStorage tlepermanentstorage;
        int tleNumber = _linesCount(tletemporarystorage.str) / 2;
	    tlepermanentstorage.nmemb = tleNumber; 
	    tlepermanentstorage.tles = (TLE *) malloc(tleNumber * sizeof(TLE));
	    _tleParse(&tletemporarystorage, &tlepermanentstorage); 

        // Cleanup
        free(tletemporarystorage.str);
        tletemporarystorage.str = NULL;    
	    return tlepermanentstorage;
}


int _tleDownload(tleTemporaryStorage* tletemporarystorage)
{
        /* Initialize libCurl */
        curl_global_init(CURL_GLOBAL_ALL);
	    CURL* curl = curl_easy_init();
	    CURLcode res;
	   
       if(!curl){
	    	fprintf(stderr, "Initialization fails\n");
	    	return -1;
	    }
        
        /* Login to Space-Track */
        char usrname[100];
        printf("Please Enter Space-Track Username: ");
        scanf("%s", usrname);
        char password[100];
        printf("Please enter Space-Track Password: ");
        scanf("%s", password);

        char *login_url = "https://www.space-track.org/ajaxauth/login";  
        char *emptykey = "identity=%s&password=%s"; 
    	char key[250];
    	snprintf(key, sizeof(key), emptykey,  usrname, password);

        // Set options for libcurl and perform PUSH request
	    curl_easy_setopt(curl, CURLOPT_URL, login_url); 
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, key);
	    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	    res = curl_easy_perform(curl);
	    if (res != CURLE_OK) { 
            fprintf(stderr, "%s \n", curl_easy_strerror(res));
    	}	

    	/* Download Data from Space-Track */ 
    	// Formulate GET request for Space-Track 
        const char *start_time = "2010-01-01";
    	const char *end_time   = "2015-08-04";

	    int NORAD_ID;
    	printf("Please enter NORAD ID: ");
    	scanf("%d", &NORAD_ID);
    	
    	static const char *raw_url =	
        "https://www.space-track.org/basicspacedata/query/class/gp_history/NORAD_CAT_ID/%d/orderby/TLE_LINE1%%20ASC/EPOCH/%s--%s/format/tle";
        char url[200];
    	snprintf(url, sizeof(url), raw_url, NORAD_ID, start_time, end_time);
	

	    curl_easy_setopt(curl, CURLOPT_URL, url);
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _tleWrite);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) tletemporarystorage);
	    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	    printf("Starting tle download and parsing ...\n");
	    res = curl_easy_perform(curl);

	    if (res != CURLE_OK){
	    	fprintf(stderr, "%s/n", curl_easy_strerror(res));
	    }

        /* Clean up */
    	curl_easy_cleanup(curl);
    	curl_global_cleanup();
    	return 0;
}


size_t _tleWrite(void *contents, size_t size, size_t nmemb, void *userp)
{
        // Get pointer to the temporary storage           
    	tleTemporaryStorage* tletemporarystorage = (tleTemporaryStorage *)userp;
    
        size_t strsize = size * nmemb;
    	char *ptr = realloc(tletemporarystorage->str, tletemporarystorage->size + strsize + 1);
    	if (ptr == NULL) {
    		printf("not enough memory (realloc returned NULL)\n");
    		return 0;
    	}

    	tletemporarystorage->str = ptr;
        memcpy(&(tletemporarystorage->str[tletemporarystorage->size]), contents, strsize);
        tletemporarystorage->size += strsize;
	    tletemporarystorage->str[tletemporarystorage->size] = 0; 
	    return strsize;
}

void _tleLineOneParse(const char *line, tleLineOne *tle1) 
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

void _tleLineTwoParse(const char *line, tleLineTwo *tle2) 
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


int _tleParse(tleTemporaryStorage *tletemporarystorage, tlePermanentStorage* tlepermanentstorage){
	
	    // Open a memory stream for reading
	    FILE *stream = fmemopen(tletemporarystorage->str, tletemporarystorage->size, "r");
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
	    		_tleLineOneParse(line, &(tlepermanentstorage->tles[tle_nmb].line1));
	    	}
	    	else if (count % 2 == 1){
	    		_tleLineTwoParse(line, &(tlepermanentstorage->tles[tle_nmb].line2));
	    		tle_nmb ++;		
	    	}
    
    		count++;
    	}

    	// Close memory stream
    	fclose(stream);
    	return 0;
}

int _linesCount(char *str)
{
	    int lines = 0;
	    for (int i = 0; str[i]; i++) {
	    	if (str[i] == '\n') lines++;
    	}
    	return lines;
}
