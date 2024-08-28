/*                                                                          
* ArchiveThrust - main.c
*
* Authors: Michiel Snoeken & Freddy Spaulding
* Created in 2024
* GNU General Public License
*
* 
*/

#include "main.h"
#include "detect_maneuvers.h"
#include "tle_download_and_parse.h"

// Dependencies 


int main() {

    tlePermanentStorage tlepermanentstorage = tleDownloadParse();
    
    multOrbParams(&tlepermanentstorage);

    return 0;
}

