#!/bin/sh

MODE="release"

# Parse command line arguments
while [ "$1" != "" ]; do
    case $1 in
        --debug ) MODE="debug"
                  ;;
        * )       echo "Invalid option: $1"
                  echo "Usage: $0 [--debug]"
                  exit 1
    esac
    shift
done

# Build the project
if [ "$MODE" = "debug" ]; then
    echo "Building in debug mode..."
    make VERSION=sdl2 debug
else
    echo "Building in release mode..."
    make VERSION=sdl2
fi

# Execute the binary
if [ -f "bin/sdl2/nibble8" ]; then
    ./bin/sdl2/nibble8
else
    echo "Error: The binary 'nibble8' does not exist."
    exit 1
fi
