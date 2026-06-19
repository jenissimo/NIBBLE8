#!/bin/bash
# Per-primitive cost sweep: measures each drawing primitive in isolation on the
# deterministic Bochs i486 harness, at the same per-frame counts as the heavy
# stress cart, and reports cost-over-baseline (cls only). Ranks the hot ops.
set -e
cd "$(dirname "$0")/../.."   # repo root
APP=tools/bench/app.lua
FRAMES="${1:-120}"

# preamble shared by every micro-cart: deterministic LCG, identical each frame
read -r -d '' PRE <<'LUA' || true
local W,H=160,120
local seed=1
local function rnd(n) seed=(seed*1103515245+12345)%2147483648 return seed%n end
function _init() end
function _update() end
LUA

emit() { # $1=name  $2=loop-body-lua
  cat > "$APP" <<LUA
$PRE
function _draw()
  seed=1
  cls(0)
$2
end
LUA
}

run() { # $1=name -> echoes "name secs"
  local secs
  secs=$(bash tools/bench_bochs.sh "$FRAMES" 2>/dev/null | sed -n 's/.*emulated secs *: *\([0-9.]*\).*/\1/p')
  printf '%-10s %s\n' "$1" "${secs:-ERR}"
}

echo "=== per-primitive sweep ($FRAMES frames, i486 @ ips=16M, deterministic) ==="
emit baseline ''                                                                             ; run baseline
emit rectfill 'for i=1,120 do rectfill(rnd(W),rnd(H),4+rnd(20),4+rnd(20),rnd(4)) end'        ; run rectfill
emit circfill 'for i=1,80  do circfill(rnd(W),rnd(H),2+rnd(10),rnd(4)) end'                  ; run circfill
emit line     'for i=1,120 do line(rnd(W),rnd(H),rnd(W),rnd(H),rnd(4)) end'                  ; run line
emit pset     'for i=1,400 do pset(rnd(W),rnd(H),rnd(4)) end'                                ; run pset
emit spr      'for i=1,150 do spr(rnd(64),rnd(W),rnd(H)) end'                                ; run spr
emit print    'for i=1,16  do print("NIBBLE8 386 BENCH",rnd(W),rnd(H),rnd(4)) end'           ; run print
