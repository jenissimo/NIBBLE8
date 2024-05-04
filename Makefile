# Main Makefile

# Automatically detect the platform
ifeq ($(OS),Windows_NT)
    SYSTEM := Windows
else
    SYSTEM := $(shell uname -s)
endif

# Define default version
VERSION ?= sdl2

# Include the specific Makefile based on the version and system
include config/Makefile.$(VERSION)