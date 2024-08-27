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

    tlePermanentStorage tlepermanentstorage = tle_download_and_parse();
    
    multOrbParams(&tlepermanentstorage);

    return 0;
}

