# Compiler
CC		:= gcc

# Compiler flags
CFLAGS  := -Iinclude -Wall -Wextra 
LIBS    := -lcurl -lm


# Source files
srcs	:= src/main.c src/tle_download_and_parse.c src/maneuvers.c

# Object files
objs	:= $(srcs:src/%.c=build/%.o)

# Executable file
exec	:= Archive-Thrust

# Default target
all: $(exec)

# Linking
$(exec): $(objs)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# 
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@
	

# Clean up
clean:
	rm -f build/*.o $(exec)
	

.PHONY: all clean

# $@ represents target of the rule.
# $^ represent all prerequisities.
# syntax: $(variable:pattern=replacement)."pattern" is a sequence of characters in a string and replaces it with replacement. % serves here as "anything bbetween".
