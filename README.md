# FOOSIE
Free Object-Oriented SDL Isometric Engine (FOOSIE) is a Free and Open-Source (FOSS) isometric game engine in C++ using SDL's built-in renderer licensed under the GNU LGPL v3.0 or later. 

## Composition
FOOSIE is composed of three main components: The Isometric Renderer, The Object Class System, and The Registry. These three work together along with the core engine library to create a fully-functional game world. 

## The Object Class System
FOOSIE uses an object class system to manage the properties of various in-game objects. This system acts similar to both Unity's Prefabs and Scriptable Objects

### Class and Sub-class
Objects have a class, and a sub-class. The class is responsible for representing the actual C++ class code of the object. It includes things such as movement code and interacts with the Object Pipeline directly.

The Subclass is a versatile child class for items that share dimilar functionality. While it can be made to execute its own code using switch statements, its main purpose is to create reproducible variants of the main class. Examples include, different tile types in a tilemap, different player skins, different food types.