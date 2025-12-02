# FOOSIE
Free Object-Oriented SDL Isometric Engine (FOOSIE) is a Free and Open-Source (FOSS) isometric game engine in C++ using SDL's input/window libraries with the OpenGL renderer, licensed under the GNU LGPL v3.0 or later. 

## Installation
So far I have only install instructions for Arch Linux, so try to translate into installing these on your own distro. 

First you need the dependencies, obviously.

Run ``` sudo pacman -Sy sdl sdl2-compat json-cpp cmake make base-devel ```.
Also, just generally just make sure you have a c++ ready dev environment. 

clone the repo with `git clone https://github.com/realblobii/FOOSIEngine/`

To build it, just navigate to the FOOSIEngine directory, and firstly, run `cmake .`, then `make`, then navigate to the `game` directory and run `build/game`. This should hopefully work. Please drop an issue if you need help, I'll try to get to you.

## Composition
FOOSIE is composed of three main components: The Isometric Renderer, The Object Class System, and The Registry. These three work together along with the core engine library to create a fully-functional game world. Below is some basic information about these three components, but if you're ready to dig deeper, check out the docs (when they're done!).

## The Object Class System
FOOSIE uses an object class system to manage the properties of various in-game objects. This system acts similar to both Unity's Prefabs and Scriptable Objects.

Objects have a class, and a sub-class. The class is responsible for representing the actual C++ class code of the object. It includes things such as movement code and interacts with the Object Pipeline directly.

The Subclass is a versatile child class for items that share similar functionality. Its main purpose is to create reproducible variants of the main class. Examples include, different tile types in a tilemap, different player skins, different food types.

## The Registry
FOOSIE uses a system called the object registry. Rather than having a child-parent system, each object within FOOSIE is equal. However, they may be tethered together and objects or classes can even have their own registries due to how the engine's Object Manager works. 

## The Isometric Renderer
FOOSIE's renderer uses OpenGL to show a 3D world space using 2d tiles in an isometric perspective. Objects' subclasses will have a list textures bound to them which the code of the object can then select from and send to the renderer. FOOSIE sends all textures and objects to the renderer in one big batch called a texture atlas to greatly speed up rendering and provide a simple fix to layering tiles in world space. 

