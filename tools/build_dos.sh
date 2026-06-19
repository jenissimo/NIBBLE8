#!/bin/bash
# Build the DOS (DJGPP) version. Run from MSYS2 (needs make/zip/find).
# DJGPP standalone toolchain is expected at C:\djgpp (from andrewwutw/build-djgpp).
set -e
cd "$(dirname "$0")/.."

DJGPP_ROOT="/c/djgpp"
export PATH="$DJGPP_ROOT/i586-pc-msdosdjgpp/bin:$DJGPP_ROOT/bin:$PATH"
export GCC_EXEC_PREFIX="$DJGPP_ROOT/lib/gcc/"
export DJDIR="$DJGPP_ROOT/i586-pc-msdosdjgpp"

mingw32-make -f config/Makefile.dos clean >/dev/null 2>&1 || true
mingw32-make -f config/Makefile.dos \
    CC=i586-pc-msdosdjgpp-gcc \
    DJGPP_PATH="$DJGPP_ROOT/"
echo "MAKE_EXIT=$?"
