from setuptools import setup, Extension
import sys



ArchiveThrust_extension = Extension(
    name                = "src",                      
    sources             = ["ArchiveThrust/tle_download_and_parse.c",    
                           "ArchiveThrust/detect_maneuvers.c"],
    include_dirs        = ["include"],                   
    libraries           = ["curl", "m", "gsl", "cblas"],    
    extra_compile_args  = ["-Wall", "-Wextra", "-fPIC"], 
    extra_link_args     = ["-shared"] if sys.platform == "win32" else ["-shared", "-Wl,-soname,archive-thrust.so"],  
                                    )

# Setup configuration
setup(
    name            = "ArchiveThrust",
    version         = "1.0.0",      
    ext_modules     = [ArchiveThrust_extension],     
    packages        = ["ArchiveThrust"],
     )
