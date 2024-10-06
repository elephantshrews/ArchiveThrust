from setuptools import setup, Extension
import sys



archive_thrust_extension = Extension(
    name                = "ArchiveThrust",                      
    sources             = ["ArchiveThrust/tle_download_and_parse.c",    
             "ArchiveThrust/detect_maneuvers.c"],
    include_dirs        = ["include"],                   
    libraries           = ["curl", "m", "gsl", "cblas"],    
    extra_compile_args  = ["-Wall", "-Wextra", "-fPIC"], 
    extra_link_args     = ["-shared"] if sys.platform == "win32" else ["-shared", "-Wl,-soname,ArchiveThrust.so"],  
                                    )

# Setup configuration
setup(
    name            = "ArchiveThrust",
    version         = "0.1.0",      
    ext_modules     = [archive_thrust_extension],     
    packages        = ["ArchiveThrust"],
     )
