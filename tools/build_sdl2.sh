#!/bin/bash
# Clean rebuild of the SDL2 (Windows/MINGW64) version.
cd "$(dirname "$0")/.."
mingw32-make clean >/dev/null 2>&1
mingw32-make VERSION=sdl2
echo "MAKE_EXIT=$?"
