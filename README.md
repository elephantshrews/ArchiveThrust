# ArchiveThrust

ArchiveThrust allows a user to obtain a maneuver history of satellites. It is a command-line application that prompts the user for a satellite identification number (NORAD ID) and their login credentials for Space-Track. The program then returns a list of the most recent maneuvers of the selected satellite. These maneuvers are computed by downloading TLE data archives and applying known algorithms to detect such maneuvers.

# Installation
To install the application, the installation of the external C/Python libraries with the code below is required.

For Fedora:
```
sudo dnf install libcurl-devel gsl-devel blas-devel python3-tkinter
```
For Ubuntu:
```
sudo apt install libcurl4-openssl-dev libgsl-dev libatlas-base-dev python3-tk
```

For Arch:

Second, clone the repository in the repository where you want to store ArchiveThrust.
```
git clone https://github.com/elephantshrews/ArchiveThrust.git
```
Third, navigate into the folder and install:
```
pipx install .
```
For that, of course, pipx needs to be installed on the machine, just like Python3.

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
