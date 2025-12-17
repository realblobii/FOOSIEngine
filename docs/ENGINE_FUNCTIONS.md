# FOOSIEngine — Engine API Reference

This document summarizes the public/usable engine functions and common integration points. It's intended for engine users and script authors (game code in `game/`) and focuses on the functions that existing game code calls.

## Key components (high level)

- `Engine` (class) — main runtime, manages window, GL context, input, objects and rendering. Defined in `engine/enginem.h` / `engine/enginem.cpp`.
- `objManager` — object factory/registry and loader (reads `assets/objects.json`). Creates object instances and resolves textures.
- `Object` — base class for game objects. Subclasses register via `ObjectFactory`.
- `mListener` / `kListener` — input listeners for mouse & keyboard with broadcast and high-level handlers (Down/Up/Hold).
- `renderPipeline` — rendering subsystem (see `engine/render/*`).

This reference documents the functions you should call from game code.

---

## Engine (engine/enginem.h)

Public members and usage:

- `Engine()` / `~Engine()`
  - Constructor / destructor.

- `void Init(const char* title, int w, int h, bool fullscreen)`
  - Initialize SDL, GL context, create `objMgr`, `renderPipeline`, and input listeners.
  - Example: `engine->Init("My Game", 800, 600, false);`
  - Side-effects: allocates `objMgr`, `rPipeline`, `mLnr`, `kLnr` if not present.

- `void handleEvents()`
  - Polls SDL events and forwards them to the input listeners. Also handles `SDL_QUIT`.
  - Call once per frame before `update()`.

- `void update()`
  - Calls every registry object's `Update()` and then ticks input listeners (this invokes Hold handlers). Also calls the global `Update()` hook from `game/main.cpp`.
  - Call each frame to progress game state.

- `void render()`
  - Triggers the render pipeline (`rPipeline->renderAll()`).

- `void clean()`
  - Tears down engine subsystems and SDL. Call on exit.

- `bool running()`
  - Convenience inline that returns engine's running state.

- `SDL_Window* getWindow()`
  - Returns the SDL window pointer for platform-specific integrations.

- `void loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight)`
  - Loads a tilemap JSON and assigns it to `tileMap` (owned by Engine).
  - Use `game/engine_api.h` wrapper `LoadTileMap()` for convenience.

Public fields you may use (beware direct mutations):
- `objManager* objMgr` — object manager; use its factory method to instantiate objects.
- `mListener* mLnr` — mouse listener (register handlers or poll)
- `kListener* kLnr` — keyboard listener
- `renderPipeline* rPipeline` — rendering pipeline (low-level)

Note: Prefer using the provided wrappers in `game/engine_api.h` (Instantiate, LoadTileMap) in scripts — they're thin convenience helpers and avoid repeating engine internals such as `engine->objMgr->instantiate(...)`.

---

## Object system

Files: `engine/obj/*` and `game/assets/objects.json`.

- `ObjectFactory::registerClass(name, creator)`
  - Register a class creator that returns a `unique_ptr<Object>`.
  - Example: `ObjectFactory::registerClass("tile", [](){ return make_unique<Tile_OBJ>(); });`

- `objManager::obj_load(class, subclass)`
  - Creates a new `Object` instance for the class and subclass.

- `objManager::instantiate(class, subclass, x, y, z)`
  - Creates an instance, assigns position and id, applies prototype properties (see below), then calls `resolveTexture()`.
  - Returns a raw pointer to the object owned by the manager's registry.

### Prototype properties (objects.json)

- `game/assets/objects.json` contains prototype entries with optional `properties` object.
- Prototypes may not override core engine-managed fields: `x`, `y`, `z`, `texref`. The engine will ignore these keys and emit a warning.
- To allow safe, flexible prototype-driven properties, `Object` now exposes a dynamic property registry:
  - Derived classes call `registerFloatProperty(name, ref)`, `registerStringProperty(name, ref)`, or `registerProperty(name, setter)` in their constructor to expose named properties.
  - When instantiating, the engine applies only registered property setters for keys found in the prototype's `properties` object.

Example (Tile class):

```cpp
// in Tile class constructor
registerFloatProperty("speed", speed);
```

And in `objects.json`:

```json
"properties": { "speed": 2.0 }
```

This will set the `speed` member on instantiated tiles.

---

## Input listeners (engine/input)

`mListener` (mouse) and `kListener` (keyboard) provide two modes:

1. Broadcast: `addHandler(fn)` receives every input event copy.
2. High-level registration: `onMouseDown("left", fn)`, `onMouseHold("left", fn)`, `onMouseUp("left", fn)` and for keyboard `onKeyDown("a", fn)`, `onKeyHold(...)`, `onKeyUp(...)`.

- Hold handlers are invoked from `Engine::update()` via `listener->tick()` and will be called every frame while the key/button is held.
- For single-consumer/call-once semantics use the polling `get()` method which returns-and-clears the last event.

Example:

```cpp
engine->kLnr->onKeyHold("w", [](const kListener::key &k){ player->y += 1.0f; });
```

---

## Render pipeline

See `engine/render/*`. The `renderPipeline` collects objects and textures into an atlas and renders them. Use `engine->rPipeline` sparingly — prefer higher-level helpers.

---

## Game-side helpers (game/engine_api.h)

Includes small inline wrappers:

-- `Object* Instantiate(const string &class, const string &subclass, float x, float y, float z)` — script-facing helper that instantiates a prototype; prefer this over calling engine internals.
-- `void LoadTileMap(const string &jsonFile, int tileWidth, int tileHeight)` — script-facing helper that loads a tilemap into the engine.

Include `game/main.h` before `game/engine_api.h` to access the global `engine` pointer.

---

## Debugging & build notes

- Quick build/run: `cd game && ./quickbnr.sh` (script cleans CMake caches, runs cmake/make and executes the game binary)
- If you change engine headers, cmake will reconfigure; if you get stale build artifacts, run `./quickbnr.sh` to clean and rebuild.

---

## Conventions & patterns

- Object classes register themselves with `ObjectFactory` in their header files (see `engine/tile/tile_oclass.h`). This static-registration pattern runs at program startup.
- Prototype data lives in `game/assets/objects.json`. Keep only prototype-specific fields in `properties`; instance placement must be provided when calling `instantiate()`.
- Input: prefer high-level `onKey*/onMouse*` registration for game behavior. Use `addHandler` for global debug hooks.

---

If you need example code or a change to the API surface (e.g., add unregister tokens, stronger validation, or per-scene prototype overrides) I can implement it next. 
