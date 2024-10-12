# ArchiveThrust

ArchiveThrust allows a user to obtain a maneuver history of satellites. It is a command-line application that prompts the user for a satellite identification number (NORAD ID) and their login credentials for Space-Track. The program then returns a list of the most recent maneuvers of the selected satellite. These maneuvers are computed by downloading TLE data archives and applying known algorithms to detect such maneuvers.

# Installation
This installation guide is specifically tailored for Fedora and Ubuntu; It has not been tested for others systems. However, it can still be used as a reference or example for other Linux distributions or Windows/MacOS with appropriate adjustments, if one desires to do so. To install the application, the installation of the external C/Python libraries with the code below is required.

For Fedora:
```
sudo dnf install libcurl-devel gsl-devel blas-devel python3-tkinter
```
For Ubuntu:
```
sudo apt install libcurl4-openssl-dev libgsl-dev libatlas-base-dev python3-tk
```

Second, clone the repository in the repository where you want to store ArchiveThrust.
```
git clone https://github.com/elephantshrews/ArchiveThrust.git
```
Third, navigate into the folder and install:
```
pipx install .
```
For that, of course, pipx needs to be installed on the machine, just like Python3.
# Configure 
You will need to set the following environment variables in your login file to run ArchiveThrust.

    ST_USERNAME space-track.org username (of the form ST_LOGIN="username")
    ST_PASSWORD space-track.org password (of the form ST_PASSWORD="password")


# Usage
To use ArchiveThrust, run `ArchiveThrust` from a command prompt after installing the software. The program will ask you for your login credentials from Space-Track.org, which you will need to type manually into the command prompt. Therefore, it is necessary to own an account from Space-Track. Afterwards, it will ask you for a NORAD ID. The software will then return recent satellite manoeuvres.
