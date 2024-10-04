import ctypes
import os


# Load the shared library
if os.name == "posix":
    lib = ctypes.CDLL("../ArchiveThrust.so") # Linux/MacOS
else:
    lib = ctypes.CDLL("../ArchiveThrust.dll")   # Windows


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
