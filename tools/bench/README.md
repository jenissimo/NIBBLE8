# Headless DOS render-loop profiling

Two harnesses run the engine's `--bench` mode (uncapped fixed-frame render loop)
headlessly and report timing parsed from `NIBBLE8.LOG`.

| Harness                | Emulator  | CPU             | Timing                         |
|------------------------|-----------|-----------------|--------------------------------|
| `tools/bench_dos.sh`   | DOSBox-X  | pentium_mmx¹    | wall-clock-ish, jittery        |
| `tools/bench_bochs.sh` | Bochs 3.0 | **i486dx4**     | **deterministic** (sync=none)  |

¹ DOSBox can't run the i486 ISA faithfully, so it fakes 386/486 *speed* via a
low cycle budget while still running a pentium-mmx binary.

## Bochs harness (recommended for before/after comparisons)

```bash
bash tools/bench_bochs.sh [frames] [ips]
#   frames  uncapped frames to render        (default 120)
#   ips     Bochs instructions/sec speed dial (default 16000000, ~486DX-33)
```

Why it's deterministic: `clock: sync=none` makes Bochs advance emulated time
purely from executed instructions at the configured `ips`, so the engine's
`uclock()` result is exactly reproducible:

```
secs = (instructions executed in the bench loop) / ips
```

`ips` is therefore a *linear speed dial* (like DOSBox `cycles`) — it scales
`secs` but not the instruction count. **Keep `ips` fixed across runs; the ratio
of `secs` is the real speedup.** Two runs at the same settings are bit-identical.

This is **not** a cycle-accurate 486 (no cache / wait-state / prefetch model).
For the authoritative per-loop *instruction count*, use a Bochs instrumentation
build (planned — see repo task list).

### How it works

Bochs can't mount a host directory, so the engine is baked into a bootable
FreeDOS image:

1. `bin/dos486/nibble8.exe` — the engine built with `-march=i486`
   (`tools/build_dos.sh` i486 target; see `config/Makefile.dos` `MARCH`).
2. `FD13LITE.img` — pristine FreeDOS 1.3 LiteUSB (downloaded, cached, gitignored).
3. The harness clones it to `fd-nibble.img`, injects `C:\NIBBLE\` (engine + cart)
   and a minimal `FDAUTO.BAT` that runs the bench then `FDAPM POWEROFF`.
4. FDAPM trips the shutdown port → Bochs exits → the harness reads `NIBBLE8.LOG`
   back out with `mtools` and parses `BENCH: loop done, secs=… fps=…`.

### One-time setup

```bash
# 1. Tools (already present on this machine):
winget install Bochs.Bochs                                   # -> C:\Program Files\Bochs-3.0
pacman -S mingw-w64-x86_64-mtools                            # MSYS2

# 2. FreeDOS base image (gitignored, ~16 MB download -> 32 MB image):
curl -o tools/bench/FD13-LiteUSB.zip \
  https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.3/official/FD13-LiteUSB.zip
(cd tools/bench && unzip -o FD13-LiteUSB.zip)               # -> FD13LITE.img

# 3. Build the i486 engine:
mingw32-make -f config/Makefile.dos CC=i586-pc-msdosdjgpp-gcc DJGPP_PATH=/c/djgpp/ \
  MARCH=i486 OBJDIR=obj/dos486 BUILDDIR=bin/dos486
```

Overridable env vars: `BOCHS` (path to bochs.exe), `MTOOLS_DIR`, `BENCH_TIMEOUT`.
