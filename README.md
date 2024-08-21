# ArchiveThrust

ArchiveThrust allows a user to obtain a maneuver history of satellites. It is a command-line application that prompts the user for a satellite identification number (NORAD ID) and their login credentials for Space-Track. The program then returns a list of the most recent maneuvers of the selected satellite. These maneuvers are computed by downloading TLE data archives and applying known algorithms to detect such maneuvers.

# Installation

To install the application, first install the external libraries with the code below.

'''
sudo dnf install libcurl-dev
'''

Second, clone the repository

'''
git clone https://github.com/elephantshrews/ArchiveThrust.git
'''

The third step is to go to the folder, build the project, and install it.

'''
cd ArchiveThrust
make
sudo make install
make clean
'''



# Uninstalling

'''
cd /path/to/ArchiveThrust
sudo make uninstall
cd ..
rm -r ArchiveThrust
'''
