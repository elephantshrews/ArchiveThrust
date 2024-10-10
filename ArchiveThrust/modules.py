from pathlib import Path
import sysconfig
import ctypes


# We need a bit of gymnastics to retrive the shared 
# library path
thisfolder = Path(__file__).parent.absolute()
suffix = sysconfig.get_config_var('EXT_SUFFIX')

# Load compiled shared library
lib = ctypes.CDLL(thisfolder.joinpath('ArchiveThrust' + suffix))

# login
# Declare the argument types and return types for the C function
lib.login.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.login.restype = ctypes.c_int

def login(username, password):
    return lib.login(username.encode('utf-8'), password.encode('utf-8'))


# create permanent storage
lib.create_permanent_storage.restype = ctypes.c_void_p
def create_permanent_storage():
    ptr = lib.create_permanent_storage()
    if ptr is None:
        raise RuntimeError("Failed to create permanent storage")
    return ptr 

lib.download.argtypes = [ctypes.c_char_p, ctypes.c_void_p]
lib.download.restype = ctypes.c_int
def download(norad_id, tlestor):
    if tlestor is None:
        raise RuntimeError("Temporary or permanent storage is None")
    return lib.download(norad_id.encode('utf-8'), tlestor)


###########
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

# Function to extract orbital parameters (useful for detectManeuvers)

# Define Maneuver structure as per your C code
# Function to detect maneuvers
lib.detectManeuvers.argtypes = [ctypes.POINTER(TleStor), ctypes.POINTER(Maneuver)]
lib.detectManeuvers.restype = None



def detect_maneuvers(tle_st, detected_maneuvers):
    # Create an array of Maneuver to hold detected maneuvers
    lib.detectManeuvers(tle_st, detected_maneuvers)


# Example usage of the function
# Assuming you have filled the TleStor structure and it's ready to be passed
# tle_st = TleStor(...)
# detect_maneuvers(tle_st)


