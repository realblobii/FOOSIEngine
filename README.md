# FOOSIE

[![Docs](https://img.shields.io/badge/docs-FOOSIEngine-blue)](https://realblobii.github.io/FOOSIEngine/)

Free Object-Oriented SDL Isometric Engine (FOOSIE) is a Free and Open-Source (FOSS) isometric game engine in C++ using SDL's input/window libraries with the OpenGL renderer, licensed under the GNU LGPL v3.0 or later. 

## Installation

### Dependencies 
Dependencies such as GLAD, stb_image, and LearnOpenGL's libraries are included with the repo with their respective licenses in the `/incl` directory, however, you will also need a few extra dependencies.
```
sdl
sdl2-compat
jsoncpp
cmake
make
gcc
```
### Quick Start
Paste this into your terminal to download, build, and run FOOSIE for the first time:
```
git clone https://github.com/realblobii/FOOSIEngine/
cd FOOSIEngine/game
./quickbnr.sh
```
The window will be blank for now, and the engine will complain that it can't find `objects.json`, but no other errors should show. Open an issue if you get an error and we'll sort it out. 

### Installation Instructions
clone the repo with `git clone https://github.com/realblobii/FOOSIEngine/`

To build it, just navigate to the `game` directory and run the `./quickbnr.sh` script to build the engine using the biolerplate code in `game/main.cpp`.

If you see this error:
```
make[1]: CMakeFiles/Makefile2: No such file or directory
make[1]: *** No rule to make target 'CMakeFiles/Makefile2'.  Stop.
make: *** [Makefile:104: clean] Error 2
make[1]: CMakeFiles/Makefile2: No such file or directory
make[1]: *** No rule to make target 'CMakeFiles/Makefile2'.  Stop.
make: *** [Makefile:104: clean] Error 2
```
Ignore it, it doesn't affect anything. 

## Composition
FOOSIE is composed of three main components: The Isometric Renderer, The Object Class System, and The Registry. These three work together along with the core engine library to create a fully-functional game world. Below is some basic information about these three components, but if you're ready to dig deeper, check out the docs (when they're done!).

For full API documentation and developer guidance see the docs: ./docs/index.md

## The Object Class System
FOOSIE uses an object class system to manage the properties of various in-game objects. This system acts similar to both Unity's Prefabs and Scriptable Objects.

Objects have a class, and a sub-class. The class is responsible for representing the actual C++ class code of the object. It includes things such as movement code and interacts with the Object Pipeline directly.

The Subclass is a versatile child class for items that share similar functionality. Its main purpose is to create reproducible variants of the main class. Examples include, different tile types in a tilemap, different player skins, different food types.

## The Registry
FOOSIE uses a system called the object registry. It's literally just a fancy way of saying a hierarchy. 

## The Isometric Renderer
FOOSIE's renderer uses OpenGL to show a 3D world space using 2d tiles in an isometric perspective. Objects' subclasses will have a list textures bound to them which the code of the object can then select from and send to the renderer. FOOSIE sends all textures and objects to the renderer in one big batch called a texture atlas to greatly speed up rendering and provide a simple fix to layering tiles in world space. 

