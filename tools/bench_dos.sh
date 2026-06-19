#!/bin/bash
# Headless DOS render benchmark harness.
# Packs the stress-test cart, runs the DOS build under DOSBox-X in --bench mode,
# and reports the timing parsed from the engine debug log.
#
# Usage: bash tools/bench_dos.sh [frames] [cycles]
#   frames  number of uncapped frames to render   (default 120)
#   cycles  DOSBox-X fixed cycle budget            (default 20000)
#
# Timing comes from nibble8.log ("BENCH: loop done, secs=.. fps=..") which the
# engine writes via --debug. We parse the log rather than bench_result.txt
# because DOSBox-X does not reliably flush files created in a mounted SUBdir to
# the host on -exit, whereas the log lives in the mount root and does sync.
#
# `secs` is the EMULATED CPU time at the chosen cycle budget. Since emulated
# time scales with the cycle budget, the run also prints an estimate for a
# 386DX-33 (~4000 cycles) so numbers are comparable to real period hardware.
# For before/after optimization comparisons, keep `cycles` fixed; the ratio of
# `secs` is the real speedup regardless of the absolute budget.
#
# Requires: a built bin/dos/nibble8.exe, DOSBox-X, zip (MSYS2).
set -e
cd "$(dirname "$0")/.."

FRAMES="${1:-120}"
CYCLES="${2:-20000}"
CYCLES_386=4000
DOSBOX_X="${DOSBOX_X:-/c/DOSBox-X/dosbox-x.exe}"
DRIVE_C="bin/dos/drive_c"
LOG="bin/dos/nibble8.log"
GENCONF="config/.dosbox-x.bench.gen.conf"
DOS_WIN_PATH='C:\Projects\NIBBLE8\bin\dos'

if [ ! -f bin/dos/nibble8.exe ]; then
    echo "ERROR: bin/dos/nibble8.exe not found. Build it first (tools/build_dos.sh)." >&2
    exit 1
fi

# Pack the cart (a .n8 is just a zip containing app.lua).
mkdir -p "$DRIVE_C"
rm -f "$DRIVE_C/bench.n8" "$LOG"
( cd tools/bench && zip -q "$OLDPWD/$DRIVE_C/bench.n8" app.lua )

cat > "$GENCONF" <<EOF
[sdl]
output      = surface
waitonerror = false
[dosbox]
machine      = svga_s3
memsize      = 16
startbanner  = false
fastbioslogo = true
quit warning = false
working directory option = noprompt
# NOTE: cputype must match the binary's instruction set. The DOS build is
# compiled with -march=pentium-mmx (config/Makefile.dos), so it emits Pentium/MMX
# opcodes and CANNOT run on cputype=386 (illegal-instruction trap -> hang). We
# therefore run on pentium_mmx and model 386-class *speed* via a low cycle
# budget. (A faithful real-386 run requires recompiling with -march=i386/i486.)
[cpu]
core    = normal
cputype = pentium_mmx
fpu     = true
cycles  = fixed $CYCLES
[mixer]
nosound = true
[sblaster]
sbtype = none
[speaker]
pcspeaker = false
[autoexec]
mount c "$DOS_WIN_PATH"
c:
nibble8.exe --cart bench --bench --bench-frames $FRAMES --debug
exit
EOF

export SDL_AUDIODRIVER=dummy
echo "Running DOSBox-X: frames=$FRAMES cycles=$CYCLES ..."
timeout 300 "$DOSBOX_X" -conf "$GENCONF" -exit >/dev/null 2>&1 || true

LINE="$(grep 'BENCH: loop done' "$LOG" 2>/dev/null | tail -1)"
if [ -z "$LINE" ]; then
    echo "ERROR: no BENCH result in $LOG — the run failed or timed out." >&2
    [ -f "$LOG" ] && { echo "--- log tail ---"; tail -8 "$LOG"; }
    exit 1
fi

SECS="$(echo "$LINE" | sed -n 's/.*secs=\([0-9.]*\).*/\1/p')"
FPS="$(echo "$LINE" | sed -n 's/.*fps=\([0-9.]*\).*/\1/p')"
EST386="$(awk -v f="$FPS" -v c="$CYCLES" -v c386="$CYCLES_386" 'BEGIN{ printf "%.2f", f*c386/c }')"

echo "=== DOS render benchmark ==="
echo "frames        : $FRAMES"
echo "cycle budget  : $CYCLES (DOSBox fixed)"
echo "emulated secs : $SECS"
echo "fps @ $CYCLES cyc : $FPS"
echo "est. 386DX-33 fps : $EST386   (scaled to ${CYCLES_386} cycles)"
