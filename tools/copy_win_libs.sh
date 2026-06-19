#!/bin/bash
# Copies the MinGW64 runtime DLLs required by the SDL2 build into assets/win_libs.
# Run from an MSYS2 MINGW64 shell.
set -e
cd "$(dirname "$0")/.."
mkdir -p assets/win_libs
for d in $(ldd bin/sdl2/nibble8.exe | grep -i '/mingw64/bin/' | awk '{print $3}'); do
    cp -u "$d" assets/win_libs/
done
ls assets/win_libs | wc -l
