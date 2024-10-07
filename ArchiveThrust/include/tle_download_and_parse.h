/*                                                                          
* ArchiveThrust - tle_download_and_parse.h
*
* Authors: Michiel Snoeken & Freddy Spaulding
* Created in 2024
* GNU General Public License
*
* 
*/
#ifndef TLE_DOWNLOAD_H
#define TLE_DOWNLOAD_H
#include "gsl/gsl_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#include "main.h"

/* Prototypes */ 
TleStor* tleDownloadParse(void);
int download(char* norad_id, void *tlestor_init);
int login(char *username, char *password);
void* create_temporary_storage(void);
void* create_permanent_storage(void);

static size_t write(void *cont, size_t size, size_t nmemb, void *userp);
static int tle_parse(TleTemp *tletemp, TleStor *tlestor);
static void tle_line_one_parse(const char *line, tleLineOne *tle1); 
static void tle_line_two_parse(const char *line, tleLineTwo *tle2);
static void cleanup_curl(CURL* curl);
static int lines_count(char *str);


#endif // TLE_DOWNLOAD_H







