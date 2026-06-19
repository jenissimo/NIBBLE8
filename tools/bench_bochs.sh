#!/bin/bash
# Headless i486 render benchmark harness, Bochs edition.
#
# Why Bochs instead of DOSBox-X (tools/bench_dos.sh):
#   * Determinism. With `clock: sync=none` Bochs advances emulated time purely
#     from executed instructions at the configured `ips`, so the engine's
#     uclock() result is exactly reproducible: secs = (instructions in the bench
#     loop) / ips. Same binary + same ips => identical secs to the microsecond.
#   * Real 486 ISA. The cart runs a binary compiled with -march=i486 (built into
#     bin/dos486 by tools/build_dos.sh's i486 target), not a pentium-mmx binary
#     pretending to be slow. No illegal-opcode risk on a true 486 model.
#
# Unlike DOSBox-X, Bochs cannot mount a host directory, so the engine is baked
# into a bootable FreeDOS HDD image (tools/bench/fd-nibble.img, cloned from the
# pristine FD13LITE.img). A minimal FDAUTO.BAT runs the bench then powers the
# machine off via FDAPM, which makes Bochs exit; we read the timing back out of
# the image's NIBBLE8.LOG with mtools.
#
# `ips` is a linear speed dial, exactly like DOSBox `cycles`: it scales secs but
# not the instruction count, so for before/after optimization comparisons keep
# it fixed and compare the `secs` ratio. The default (~16 MIPS) loosely models a
# 486DX-33; it is NOT a cycle-accurate 486 (no cache/wait-state model). For the
# authoritative per-loop instruction count use the instrumentation build.
#
# Usage: bash tools/bench_bochs.sh [frames] [ips]
#   frames  number of uncapped frames to render   (default 120)
#   ips     Bochs instructions/sec (speed dial)   (default 16000000)
#
# Requires: bin/dos486/nibble8.exe (tools/build_dos.sh i486 target), Bochs 3.0,
# mtools (MSYS2 mingw64), zip, and tools/bench/FD13LITE.img (FreeDOS 1.3 LiteUSB).
set -e
cd "$(dirname "$0")/.."

FRAMES="${1:-120}"
IPS="${2:-16000000}"

BOCHS="${BOCHS:-/c/Program Files/Bochs-3.0/bochs.exe}"
# Bochs is a native Windows binary: paths inside the bochsrc must be in Windows
# form (C:/...), NOT MSYS form (/c/...). An MSYS path silently fails to load the
# BIOS and the CPU spins in a reset loop until the timeout.
BOCHS_DIR="$(cygpath -m "$(dirname "$BOCHS")" 2>/dev/null || dirname "$BOCHS")"
MTOOLS_DIR="${MTOOLS_DIR:-/c/msys64/mingw64/bin}"
export PATH="$MTOOLS_DIR:$PATH"
export MTOOLS_SKIP_CHECK=1

B=tools/bench
BASE="$B/FD13LITE.img"          # pristine FreeDOS 1.3 LiteUSB image (cached)
IMG="$B/fd-nibble.img"          # working image with the engine baked in
PART_OFF=32256                  # byte offset of the FAT16 partition in the image
PART="$IMG@@$PART_OFF"
BUILD=bin/dos486
LOG_IMG="::/NIBBLE/NIBBLE8.LOG"
LOG_HOST="$B/nibble8.bochs.log"
GENCONF=config/.bochsrc.bench.gen
WINROOT="$(pwd -W)"             # Windows-form repo root, for Bochs paths

# --- preflight ---------------------------------------------------------------
[ -f "$BOCHS" ]              || { echo "ERROR: Bochs not found at $BOCHS (set \$BOCHS)." >&2; exit 1; }
[ -x "$MTOOLS_DIR/mcopy" ] || command -v mcopy >/dev/null || { echo "ERROR: mtools (mcopy) not found in $MTOOLS_DIR." >&2; exit 1; }
[ -f "$BUILD/nibble8.exe" ] || { echo "ERROR: $BUILD/nibble8.exe not found. Build the i486 target:" >&2
    echo "  mingw32-make -f config/Makefile.dos CC=i586-pc-msdosdjgpp-gcc DJGPP_PATH=/c/djgpp/ MARCH=i486 OBJDIR=obj/dos486 BUILDDIR=bin/dos486" >&2; exit 1; }
[ -f "$BASE" ]              || { echo "ERROR: $BASE not found. Fetch FreeDOS 1.3 LiteUSB:" >&2
    echo "  curl -o $B/FD13-LiteUSB.zip https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.3/official/FD13-LiteUSB.zip && (cd $B && unzip -o FD13-LiteUSB.zip)" >&2; exit 1; }

# --- (re)build the working image when the engine is newer than the image -----
if [ ! -f "$IMG" ] || [ "$BUILD/nibble8.exe" -nt "$IMG" ]; then
    echo "Preparing working image from $BASE ..."
    cp -f "$BASE" "$IMG"
    mmd   -i "$PART" ::/NIBBLE ::/NIBBLE/drive_c
    mcopy -i "$PART" -o "$BUILD/nibble8.exe" "$BUILD/CWSDPMI.EXE" "$BUILD/config.ini" "$BUILD/rom.zip" ::/NIBBLE/
fi

# --- pack the cart (a .n8 is a zip of app.lua) -------------------------------
rm -f "$B/bench.n8"
if [ -n "$CART_N8" ]; then
    # use a prebuilt cart (e.g. a demo) instead of packing tools/bench/app.lua
    cp "$CART_N8" "$B/bench.n8"
else
    # include music.mod in the cart when present (for audio profiling), else just app.lua
    ( cd "$B" && zip -q -X bench.n8 app.lua $([ -f music.mod ] && echo music.mod) )
fi
mcopy -i "$PART" -o "$B/bench.n8" ::/NIBBLE/drive_c/

# --- per-run autoexec: run the bench with this frame count, then power off ---
printf '@echo off\r\nSET DOSDIR=\\FREEDOS\r\nSET PATH=%%DOSDIR%%\\BIN\r\nC:\r\nCD \\NIBBLE\r\nnibble8.exe --cart bench --bench --bench-frames %s --debug\r\n\\FREEDOS\\BIN\\FDAPM.COM POWEROFF\r\n' "$FRAMES" > "$B/FDAUTO.BAT"
mcopy -i "$PART" -o "$B/FDAUTO.BAT" ::/FDAUTO.BAT
mdel  -i "$PART" "$LOG_IMG" 2>/dev/null || true

# --- generate the bochsrc ----------------------------------------------------
cat > "$GENCONF" <<EOF
# Auto-generated by tools/bench_bochs.sh — do not edit by hand.
# Headless i486 profiling profile. clock sync=none + fixed ips => deterministic
# uclock timing: secs = (instructions in the bench loop) / ips.
romimage:    file="$BOCHS_DIR/BIOS-bochs-latest"
vgaromimage: file="$BOCHS_DIR/VGABIOS-lgpl-latest.bin"
cpu:    model=i486dx4, ips=$IPS, reset_on_triple_fault=1
clock:  sync=none, time0=0
megs:   16
ata0:        enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
ata0-master: type=disk, path="$WINROOT/$IMG", mode=flat, cylinders=65, heads=16, spt=63
boot:   disk
display_library: nogui
log:    $WINROOT/$B/bochs.log
panic:  action=report
error:  action=report
info:   action=ignore
debug:  action=ignore
EOF

# --- run ---------------------------------------------------------------------
# Normal exit path: FDAPM POWEROFF in FDAUTO.BAT trips the shutdown port and
# Bochs exits on its own within seconds. The watchdog only matters for a boot
# or crash loop (e.g. a bad path). Note: `timeout` can't reap the native
# Windows process (it detaches), so we background Bochs, `wait` for the normal
# exit, and taskkill as the hammer. Bochs also leaves a stale "<img>.lock" file
# when killed, which would panic the next run with "image locked" — clear it on
# both sides of the run.
DEADLINE="${BENCH_TIMEOUT:-300}"
taskkill //F //IM bochs.exe >/dev/null 2>&1 || true
rm -f "$IMG.lock"

echo "Running Bochs: frames=$FRAMES ips=$IPS (cpu=i486dx4) ..."
"$BOCHS" -q -f "$GENCONF" >/dev/null 2>&1 &
BPID=$!
# Poll in this shell (no watchdog subshell — one would inherit our stdout and a
# lingering sleep child could keep the pipe open long after Bochs exits). Breaks
# within ~1s of the FDAPM poweroff; caps at DEADLINE for a boot/crash loop.
t=0
while kill -0 "$BPID" 2>/dev/null; do
    sleep 1; t=$((t + 1))
    [ "$t" -ge "$DEADLINE" ] && { echo "WARN: Bochs exceeded ${DEADLINE}s — killing." >&2; break; }
done
taskkill //F //IM bochs.exe >/dev/null 2>&1 || true
wait "$BPID" 2>/dev/null || true
rm -f "$IMG.lock"

# --- extract and parse the timing -------------------------------------------
rm -f "$LOG_HOST"
mcopy -i "$PART" -n "$LOG_IMG" "$LOG_HOST" 2>/dev/null || true
LINE="$(grep 'BENCH: loop done' "$LOG_HOST" 2>/dev/null | tail -1)"
if [ -z "$LINE" ]; then
    echo "ERROR: no BENCH result — the run failed or timed out." >&2
    [ -f "$LOG_HOST" ] && { echo "--- log tail ---"; tail -8 "$LOG_HOST"; }
    exit 1
fi

SECS="$(echo "$LINE" | sed -n 's/.*secs=\([0-9.]*\).*/\1/p')"
FPS="$(echo "$LINE" | sed -n 's/.*fps=\([0-9.]*\).*/\1/p')"
INSTR="$(awk -v s="$SECS" -v i="$IPS" 'BEGIN{ printf "%.0f", s*i }')"

echo "=== Bochs i486 render benchmark ==="
echo "frames           : $FRAMES"
echo "cpu / ips         : i486dx4 / $IPS"
echo "emulated secs     : $SECS   (deterministic)"
echo "fps @ this ips    : $FPS"
echo "loop instructions : ~$INSTR   (secs*ips; exact count needs the instrumentation build)"
