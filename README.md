# ArchiveThrust

ArchiveThrust allows a user to obtain a maneuver history of satellites. It is a command-line application that prompts the user for a satellite identification number (NORAD ID) and their login credentials for Space-Track. The program then returns a list of the most recent maneuvers of the selected satellite. These maneuvers are computed by downloading TLE data archives and applying known algorithms to detect such maneuvers.

# Installation
This installation guide is specifically tailored for Fedora; however, it can be used as a reference or example for other Linux distributions or Windows/MacOS with appropriate adjustments. To install the application, the installation of the external C libraries with the code below is required.
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
To use ArchiveThrust, run `ArchiveThrust` from a command prompt after installing the software. The program will ask you for your login credentials from Space-Track.org, which you will need to type manually into the command prompt. Therefore, it is necessary to own an account from Space-Track. Afterwards, it will ask you for a NORAD ID. The software will then return recent satellite manoeuvres.
