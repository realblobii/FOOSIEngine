# Build, run and debugging notes

- Quick runner: from `game/` run `./quickbnr.sh`. It cleans CMake state, runs `cmake ..`/`cmake .` then `make` and finally runs the built `game` binary.
- If you edit headers, cmake reconfigures; if builds behave oddly, run `./quickbnr.sh` to force a clean configure.
- Common dependencies: SDL2 (pkg-config name `sdl2`), jsoncpp, OpenGL, cmake, make, a C++ toolchain.
- If you get linker errors for missing symbols, ensure `engine/CMakeLists.txt` includes all engine source files (we added `input/keyboard.cpp` earlier).

Runtime notes
- The engine prints warnings when prototypes attempt to set core fields (`x,y,z,texref`).
- Input handlers run on the main thread; avoid blocking operations in callbacks.
