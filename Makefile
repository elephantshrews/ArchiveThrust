# Compiler
CC      := gcc

# Compiler flags
CFLAGS  := -Iinclude -Wall -Wextra -fPIC   # -fPIC is necessary for shared libraries
LIBS    := -lcurl -lm -lgsl -lcblas

# Folders for installing
prefix  := /usr/local
bindir  := $(prefix)/bin
libdir  := $(prefix)/lib

# Source files
srcs    := src/tle_download_and_parse.c src/detect_maneuvers.c

# Object files
objs    := $(srcs:src/%.c=build/%.o)

# Shared library target (.so for Linux, .dll for Windows)
ifeq ($(OS),Windows_NT)
    shared_lib := ArchiveThrust.dll
    shared_lib_flags := -shared -o $(shared_lib)
else
    shared_lib := ArchiveThrust.so
    shared_lib_flags := -shared -o $(shared_lib) -Wl,-soname,$(shared_lib)
endif

# Default target
all: $(shared_lib)

# Linking shared library
$(shared_lib): $(objs)
	$(CC) $(shared_lib_flags) $^ $(LIBS)

# Compiling object files
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Install the shared library
install: $(shared_lib)
	install -Dm755 $(shared_lib)  $(libdir)/$(shared_lib)

# Uninstall the shared library
uninstall:
	rm -f $(libdir)/$(shared_lib)

# Clean up
clean:
	rm -f build/*.o $(shared_lib)

.PHONY: all clean install uninstall
