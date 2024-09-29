# This file is

import ctypes
import os

# Load the shared library

lib = ctpyes.CDLL("./libmylib.so")

lib.login.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.login.restype = ctypes.c_int

def login(username, password):
    return lib.loging(username, password)




username = "m.snoeken@campus.tu-berlin.de"
password = "UfQx95rK5Bj4haRhiKBP"
