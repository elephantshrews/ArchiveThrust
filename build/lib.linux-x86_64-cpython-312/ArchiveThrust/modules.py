#
#
# Archive Thrust - modules.py
#
# Authors: Michiel Snoeken & Freddy Spaulding
# GNU General Public License
#
#
#

# Packages needed to convert C-functions to Python functions
from pathlib import Path
import sysconfig
import ctypes


# Some gymnastics to retrive the shared library path 
thisfolder = Path(__file__).parent.absolute()
suffix = sysconfig.get_config_var('EXT_SUFFIX')

# Load compiled shared library
lib = ctypes.CDLL(thisfolder.joinpath('ArchiveThrust' + suffix))

################################################################
# Now, all individual functions will be translated from
# Python to C
################################################################

# login
# Declare the argument types and return types for the C function
lib.login.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.login.restype = ctypes.c_int

def login(username, password):
    return lib.login(username.encode('utf-8'), password.encode('utf-8'))


# create permanent storage
# Declare the argument types and return types for the C function
lib.create_permanent_storage.restype = ctypes.c_void_p
def create_permanent_storage():
    ptr = lib.create_permanent_storage()
    if ptr is None:
        raise RuntimeError("Failed to create permanent storage")
    return ptr 


# download
# Declare the argument types and return types for the C function
lib.download.argtypes = [ctypes.c_char_p, ctypes.c_void_p]
lib.download.restype = ctypes.c_int
def download(norad_id, tlestor):
    if tlestor is None:
        raise RuntimeError("Temporary or permanent storage is None")
    return lib.download(norad_id.encode('utf-8'), tlestor)


## Following classes are needed for the detect_meanuever## Following classes are needed for the detect_meanueverss
class TleLineOne(ctypes.Structure):
    _fields_ = [
        ("lineNumber", ctypes.c_int),
        ("satelliteNumber", ctypes.c_int),
        ("classification", ctypes.c_char),
        ("launchYear", ctypes.c_int),
        ("launchNumber", ctypes.c_int),
        ("launchPiece", ctypes.c_char * 4),
        ("epochYear", ctypes.c_int),
        ("epochDay", ctypes.c_float),
        ("meanMotionDerivative", ctypes.c_float),
        ("meanMotionSecondDer", ctypes.c_float),
        ("bstar", ctypes.c_float),
        ("ephemerisType", ctypes.c_int),
        ("elementSetNumber", ctypes.c_int),
        ("checksum", ctypes.c_int),
    ]

class TleLineTwo(ctypes.Structure):
    _fields_ = [
        ("lineNumber", ctypes.c_int),
        ("satelliteNumber", ctypes.c_int),
        ("inclination", ctypes.c_float),
        ("raan", ctypes.c_float),
        ("eccentricity", ctypes.c_float),
        ("argPerigee", ctypes.c_float),
        ("meanAnomaly", ctypes.c_float),
        ("meanMotion", ctypes.c_float),
        ("revNumber", ctypes.c_int),
        ("checksum", ctypes.c_int),
    ]

class TLE(ctypes.Structure):
    _fields_ = [
        ("line1", TleLineOne),
        ("line2", TleLineTwo),
    ]

class TleTemp(ctypes.Structure):
    _fields_ = [
        ("str", ctypes.POINTER(ctypes.c_char)),
        ("size", ctypes.c_size_t),
    ]

class TleStor(ctypes.Structure):
    _fields_ = [
        ("tles", ctypes.POINTER(TLE)),
        ("nmemb", ctypes.c_size_t),
    ]

class Maneuver(ctypes.Structure):
    _fields_ = [
        ("startEpochDay", ctypes.c_double),
        ("endEpochDay", ctypes.c_double),
        ("epochYear", ctypes.c_double),
        ("affectedParams", ctypes.c_bool * 6),
        ("fluctuations", ctypes.c_double * 6),
        ("maneuverType", ctypes.c_int * 2),
        ("confidenceLevel", ctypes.c_double),
    ]
# download
# Declare the argument types and return types for the C function
lib.detectManeuvers.argtypes = [ctypes.POINTER(TleStor), ctypes.POINTER(Maneuver)]
lib.detectManeuvers.restype = None
def detect_maneuvers(tle_st, detected_maneuvers):
    lib.detectManeuvers(tle_st, detected_maneuvers)


