# Main Makefile

# Define default version
VERSION ?= sdl2

# Define default architecture
ARCH ?= 

# Include the specific Makefile based on the version and system
include config/Makefile.$(VERSION)