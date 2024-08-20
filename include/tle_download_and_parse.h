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
int tle_parse(temp_storage *temp_stor, tle_storage* tle_stor);
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
int tle_download(const char *usrname, const char *password, temp_storage* temp_stor);
void parseTLELine2(const char *line, TLE_Line2 *tle2);
void parseTLELine1(const char *line, TLE_Line1 *tle1); 
void tle_print(TLE *tle);
tle_storage tle_download_and_parse(void);
#endif // TLE_DOWNLOAD_H

