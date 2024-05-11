# Main Makefile

# Automatically detect the platform
ifeq ($(OS),Windows_NT)
    SYSTEM := Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        SYSTEM := Linux
    endif
    ifeq ($(UNAME_S),Darwin)
        SYSTEM := Darwin
    endif
endif

# Define default version
VERSION ?= sdl2

# Define default architecture
ARCH ?= 

# Include the specific Makefile based on the version and system
include config/Makefile.$(VERSION)