# ArchiveThrust

ArchiveThrust allows a user to obtain a maneuver history of satellites. It is a command-line application that prompts the user for a satellite identification number (NORAD ID) and their login credentials for Space-Track. The program then returns a list of the most recent maneuvers of the selected satellite. These maneuvers are computed by downloading TLE data archives and applying known algorithms to detect such maneuvers.

# Installation

To install the application, first install the external libraries with the code below.
```
sudo dnf install libcurl-devel gsl-devel blas-devel
```
Second, clone the repository in the repository where you want to store ArchiveThrust.
```
git clone https://github.com/elephantshrews/ArchiveThrust.git
```
Third, go into the folder, build the project, and install it.
```
cd ArchiveThrust
make
sudo make install
make clean
```


# Uninstalling
To uninstall ArchiveThrust, run following commands.
```
cd /path/to/ArchiveThrust
sudo make uninstall
cd ..
rm -r ArchiveThrust
```

# Usage
To use ArchiveThrust, run `ArchiveThrust` from a command prompt after installing the software. The program will ask you for a NORAD ID, which you will need to type manually into the command prompt. The software will then return recent satellite manoeuvres.
