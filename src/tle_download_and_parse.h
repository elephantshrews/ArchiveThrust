#ifndef TLE_DOWNLOAD_H
#define TLE_DOWNLOAD_H
#include "velocities.h"
#include "utils.h"
#include "gsl/gsl_matrix.h"

int tle_parse(temp_storage *temp_stor, tle_storage* tle_stor);
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
int tle_download(const char *usrname, const char *password, temp_storage* temp_stor);
void parseTLELine2(const char *line, TLE_Line2 *tle2);
void parseTLELine1(const char *line, TLE_Line1 *tle1); 
void tle_print(TLE *tle);
tle_storage tle_download_and_parse(void);
#endif // TLE_DOWNLOAD_H

