#!/usr/bin/env bash
set -euo pipefail

# quickbnr.sh - quick build & run helper
# Usage: quickbnr.sh [-n|-g] [-c] [-r|--no-run]
#  -n, --ninja   : use Ninja (default)
#  -g, --gmake   : use GNU Make
#  -c, --clean   : clean build files first (runs cmake-clean.sh)
#  -r, --run     : run the built game (default)
#  --no-run      : build only, do not run

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

GENERATOR="Ninja"
RUN=true
CLEAN=false

# Parse flags
while [[ $# -gt 0 ]]; do
  case "$1" in
    -n|--ninja)
      GENERATOR="Ninja"; shift;;
    -g|--gmake)
      GENERATOR="Unix Makefiles"; shift;;
    -c|--clean)
      CLEAN=true; shift;;
    -r|--run)
      RUN=true; shift;;
    --no-run)
      RUN=false; shift;;
    -h|--help)
      echo "Usage: $0 [-n|--ninja] [-g|--gmake] [-c|--clean] [-r|--run|--no-run]"; exit 0;;
    *)
      echo "Unknown option: $1"; echo "Use -h for help"; exit 2;;
  esac
done

# Clean if requested
if [ "$CLEAN" = true ]; then
  ./cmake-clean.sh
fi

# Configure only if build files are missing (faster) or after clean
if [ ! -f CMakeCache.txt ] || [ "$CLEAN" = true ]; then
  if [ "$GENERATOR" = "Ninja" ]; then
    cmake . -G Ninja
  else
    cmake . -G "Unix Makefiles"
  fi
fi

# Build
if [ "$GENERATOR" = "Ninja" ]; then
  ninja -j$(nproc)
else
  make -j$(nproc)
fi

# Optionally run the built demo
if [ "$RUN" = true ]; then
  exec ./game/build/game
else
  echo "Build completed. Run ./game/build/game to start the demo."
fi
