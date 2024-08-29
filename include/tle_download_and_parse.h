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
tlePermanentStorage tleDownloadParse(void);
int _tleDownload(tleTemporaryStorage* temp_stor);
size_t _tleWrite(void *contents, size_t size, size_t nmemb, void *userp);
int _tleParse(tleTemporaryStorage *temp_stor, tlePermanentStorage* tle_stor);
void _tleLineOneParse(const char *line, tleLineOne *tle1); 
void _tleLineTwoParse(const char *line, tleLineTwo *tle2);
int _linesCount(char *str);



#endif // TLE_DOWNLOAD_H







