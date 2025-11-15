# FOOSIE
Free Object-Oriented SDL Isometric Engine (FOOSIE) is a Free and Open-Source (FOSS) isometric game engine in C++ using SDL's built-in renderer licensed under the GNU LGPL v3.0 or later. 

## Installation
Right now I dont have it set up properly for other people, my bad. 

## Composition
FOOSIE is composed of three main components: The Isometric Renderer, The Object Class System, and The Registry. These three work together along with the core engine library to create a fully-functional game world. Below is some basic information about these three components, but if you're ready to dig deeper, check out the docs (when they're done!).

## The Object Class System
FOOSIE uses an object class system to manage the properties of various in-game objects. This system acts similar to both Unity's Prefabs and Scriptable Objects.

### Class and Sub-class
Objects have a class, and a sub-class. The class is responsible for representing the actual C++ class code of the object. It includes things such as movement code and interacts with the Object Pipeline directly.

The Subclass is a versatile child class for items that share similar functionality. Its main purpose is to create reproducible variants of the main class. Examples include, different tile types in a tilemap, different player skins, different food types.

Subclasses for the most part aren't meant to define their own code, however, if you really need it, you can always use switch statements in the respective Object Class' code. One thing we plan to add to subclasses in the future is them determining their own variable amounts. While they won't be able to initiate new variables, they will be able to have custom amounts for some of the overarching Object Class' variables (not core engine things like coords or ID though).

### Object Loader
Generally, object classes are created in a JSON file, and defined in a header/CPP file. The JSON file mainly initiates the objects and gives things like textures or variables to sub-classes, whereas the C++ will ensure the main class will work in the engine. If you don't have a class in both JSON and C++, the Object Loader will tend to fail. We plan to automate this in future versions via commandline TUI, or a bespoke GUI.