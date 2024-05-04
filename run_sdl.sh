#!/bin/sh

# Default values
MAKEFILE="Makefile.sdl2" # Default Makefile
MODE="release"

# Parse command line arguments
while [ "$1" != "" ]; do
    case $1 in
        --makefile ) shift
                     case $1 in
                         sdl2 ) MAKEFILE="Makefile.sdl2"
                                ;;
                         sdl ) MAKEFILE="Makefile.sdl"
                               ;;
                         * )    echo "Invalid makefile option: $1"
                                echo "Usage: $0 [--makefile sdl2|sdl] [--debug]"
                                exit 1
                     esac
                     ;;
        --debug ) MODE="debug"
                  ;;
        * )       echo "Invalid option: $1"
                  echo "Usage: $0 [--makefile sdl2|sdl] [--debug]"
                  exit 1
    esac
    shift
done

# Build the project
if [ "$MODE" = "debug" ]; then
    echo "Building in debug mode using $MAKEFILE..."
    make -f $MAKEFILE debug
else
    echo "Building in release mode using $MAKEFILE..."
    make -f $MAKEFILE
fi

# Navigate to the binary directory
cd bin || exit

# Execute the binary
if [ -f "nibble8" ]; then
    ./nibble8
else
    echo "Error: The binary 'nibble8' does not exist."
    exit 1
fi
