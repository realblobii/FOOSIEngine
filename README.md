# FOOSIE — Free Object-Oriented SDL Isometric Engine

[![Docs](https://img.shields.io/badge/docs-FOOSIEngine-blue)](https://realblobii.github.io/FOOSIEngine/)  [![License](https://img.shields.io/badge/license-LGPL%20v3-blue)](LICENSE)

FOOSIE is an open-source isometric game engine in C++ using SDL2 and OpenGL. It provides a small, composable object-class system, a lightweight scene format, and an extensible rendering pipeline.

## Quick Start
Prerequisites (typical on Linux):
- cmake, ninja or make, GNU toolchain (g++), SDL2, jsoncpp, freetype2

Clone, build and run:

```bash
git clone https://github.com/realblobii/FOOSIEngine.git
cd FOOSIEngine
# Build and run with Ninja (recommended):
./quickbnr.sh -n         # build & run
# Build only:
./quickbnr.sh -n --no-run
# Clean-build:
./quickbnr.sh -n -c
```

If you prefer GNU Make use `-g`.

## Project Overview
- Object system: class+subclass prototypes (data driven via `game/demo/objects.json`) and per-instance placement via `Instantiate()`.
- Scene format: human-editable `.fscn` files supporting nested child blocks and scene references (`docs/scene.md`).
- Rendering: simple atlas-based batching suitable for isometric tilemaps and sprites.

## Where to look next
- Read the Beginner Guide: `docs/beginner.md`
- API reference and engine internals: `docs/ENGINE_FUNCTIONS.md`
- Scene format details: `docs/scene.md`

## Contributing
- Run tests (manual): build & run the demo and inspect `game/demo/scn/*` scenes.
- Style: follow existing conventions in `docs/conventions.md`.

For full documentation visit the docs site linked at the top of this README. If you encounter issues, open an issue on GitHub and include reproduction steps and logs. 

