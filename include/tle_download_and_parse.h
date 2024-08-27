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

int count_lines(char *str);
int tle_parse(tleTemporaryStorage *temp_stor, tlePermanentStorage* tle_stor);
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
int tle_download(const char *usrname, const char *password, tleTemporaryStorage* temp_stor);
void parseTLELine2(const char *line, tleLineTwo *tle2);
void parseTLELine1(const char *line, tleLineOne *tle1); 
void tle_print(TLE *tle);
tlePermanentStorage tle_download_and_parse(void);
#endif // TLE_DOWNLOAD_H

