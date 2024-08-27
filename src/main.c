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

    tle_storage tle_stor = tle_download_and_parse();
    
    multOrbParams(&tle_stor);

    return 0;
}

