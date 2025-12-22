# Beginner Guide

This quick-start guide helps you get a small FOOSIEngine project running and shows common tasks for new contributors.

## Prerequisites
- Ideally a linux-based operating system. Build tools: `cmake`, `make` or Ninja, `g++`.
- SDL2 and jsoncpp installed (your distro package manager provides these).

## Build & Run
1. From the repository root run:

   ./quickbnr.sh --ninja

   This configures, builds and runs the demo binary. Use `--gmake` to prefer GNU Make.

2. The demo loads `game/demo/scn/test.fscn` on startup which demonstrates scene loading and nested children.

## Editing scenes
- Scene files live under `game/demo/scn/`.
- Syntax basics (see `docs/scene.md` for full reference):
  - `#SCNDEF <name>` header and `#ENDSCN` footer
  - `OBJECT tile.grass 10 10 0;` — instantiate an object at coordinates (x,y,z)
  - Child blocks (inline nesting):

    OBJECT tile.parent 10 10 0
    {
        OBJECT tile.child 1 1 1;
    };

  - `SCENE <file.fscn> x y z;` — reference another scene (child scenes are attached as references when loading).

- Important: scene files must use object prototypes defined in `game/demo/objects.json`. If you reference an unknown subclass `Instantiate()` will return `nullptr` and the loader will skip the entry.

## Working with objects (sample code)
Here's a short example that demonstrates common operations you can try in `game/main.cpp` or a small test program:

```cpp
// instantiate a tile at (10,10,0)
Object* t = Instantiate("tile", "grass", 10, 10, 0);

// create a parent object and attach a child
Object* parent = Instantiate("tile", "pTest", 5, 5, 0);
Object* child = Instantiate("tile", "grass", 1, 1, 1);
engine->objMgr->addChild(parent, child);

// print current object tree to stdout
engine->objMgr->printTree(engine->objMgr->getRoot());

// save the current scene (writes to demo/scn/<name>.fscn)
engine->sceneMgr->saveScene("test");
```

- Notes:
  - Use `Instantiate()` (from `game/engine_api.h`) rather than constructing objects directly.
  - The object manager owns all instantiated objects; you should not delete them manually.

## Try it (quick exercise)
1. Open `game/main.cpp` and add the sample code above after `engine->Init(...)` to create objects at runtime.
2. Build with `./quickbnr.sh -n` and run; inspect the logged object tree and the generated `game/demo/scn/test.fscn` file.
3. Edit the saved `.fscn` file and reload to verify the loader handles nested child syntax.

## Where to look next
- `docs/scene.md` — full syntax and save/load semantics
- `docs/objects.md` — how prototypes and properties work
- `engine/obj/*` — object implementation & factory pattern
