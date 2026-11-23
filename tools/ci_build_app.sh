#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SDL_SDK="${SDL_SDK:-$ROOT/../cart/vendor/SDL}"
CC_BIN="${CC:-gcc}"
OUT="${1:-$ROOT/build/cart-sdl/linux/wolf3d-revive}"

if [[ ! -f "$SDL_SDK/build/libSDL3.a" ]]; then
  echo "FAIL: SDL static library not found: $SDL_SDK/build/libSDL3.a" >&2
  exit 2
fi
if ! command -v "$CC_BIN" >/dev/null 2>&1; then
  echo "FAIL: compiler not found: $CC_BIN" >&2
  exit 2
fi

mkdir -p "$(dirname "$OUT")"

# Flags mirror cart.toml [linux] + cart-sdl static SDL profile.
srcs=()
while IFS= read -r f; do srcs+=("$f"); done < <(find "$ROOT/src" -name '*.c' | sort)

echo "START build app -> $OUT" >&2
"$CC_BIN" -std=c11 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
  -I"$ROOT/src" -I"$SDL_SDK/include" \
  "${srcs[@]}" \
  "$SDL_SDK/build/libSDL3.a" -pthread -lm \
  -o "$OUT"

if [[ ! -x "$OUT" && ! -f "$OUT" ]]; then
  echo "FAIL: expected executable missing: $OUT" >&2
  exit 1
fi

echo "PASS built $OUT" >&2
