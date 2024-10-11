from setuptools import setup, find_packages, Extension
import sys



ArchiveThrust_extension = Extension(
    name                = "ArchiveThrust.ArchiveThrust",                      
    sources             = ["ArchiveThrust/src/tle_download_and_parse.c",    
                           "ArchiveThrust/src/detect_maneuvers.c"],
    include_dirs        = ["ArchiveThrust/include"],                   
    libraries           = ["curl", "m", "gsl", "cblas"],    
    extra_compile_args  = ["-Wall", "-Wextra", "-fPIC"], 
    extra_link_args     = ["-shared"] if sys.platform == "win32" else ["-shared", "-Wl,-soname,ArchiveThrust.so"],  
                                    )

# Setup configuration
setup(
    name            = "ArchiveThrust",
    version         = "1.0.0",      
    ext_modules     = [ArchiveThrust_extension],     
    packages        = find_packages(),
    install_requires=[
            'matplotlib',
            ]
     )
