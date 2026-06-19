# Changelog

## [Unreleased]

### Added
- `points(t, [col])` — batch pixel plot that draws many pixels in a single
  Lua→C call (the boundary that dominates pixel-dense carts). ~2.4× faster than
  a `pset` loop on the DOS build. See [API_REFERENCE.md](API_REFERENCE.md).
- Headless, deterministic DOS benchmark/profiling harness under `tools/`
  (Bochs i486 + DOSBox), a per-primitive sweep, and a framebuffer-checksum gate
  in `--bench` mode used to prove optimizations preserve output bit-for-bit.
  See [tools/bench/README.md](tools/bench/README.md).

### Changed — performance (DOS / 486)
- Rendering: byte/dword span fills for `hline`/`rectfill`, a 256-entry LUT for
  the framebuffer unpack, 4-side culling in `map()`, and a sprite/tile fast path
  (clip-once + single palette lookup + alignment-independent packed-byte blit).
  The `map_ex` demo goes from ~12 to ~30 fps on the i486 model (×2.5), with
  bit-identical output.
- Audio: DOS sample rate lowered to 11.025 kHz; the pocketmod `_u8` renderer is
  now integer fixed-point (8.8 level + 20.12 position over int8 samples), cutting
  music-render time ~55%; mixing sums only active sources in signed space; the
  audio stream is refilled independently of the 30 FPS draw tick.
- Build (DOS): default optimization tuned to `-O2` (was `-O3`), dropped
  `-funroll-all-loops`, kept `-ffloat-store` (load-bearing for x87 determinism).
  New overridable knobs: `MARCH`, `OPTFLAGS`, `PG` (gprof), `OBJDIR`/`BUILDDIR`.

### Fixed
- `cls()` now marks the frame dirty, so a cls-only frame is presented.
- Audio mixing no longer stacks DC from silent SFX channels (which overflowed
  the 8-bit mix before the limiter).
