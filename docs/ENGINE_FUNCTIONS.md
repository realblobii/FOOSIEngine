# FOOSIEngine — Engine API Reference

Location: `game/engine_api.h`, `engine/enginem.h`

Purpose
- Reference for game-facing helper functions and notable engine APIs that game code commonly calls.

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
  - Computes per-frame delta time and FPS, then calls each registry object's `UpdateDelta(float dt)` (default implementation calls the legacy `Update()` to preserve backwards compatibility), ticks input listeners (this invokes Hold handlers), and finally calls the global `Update()` hook from `game/main.cpp`.
  - `Engine` exposes `float getDeltaT()` and `float getFPS()` to access the last frame's delta and the current FPS estimate respectively. `getDeltaT()` is the recommended way for object updates and movement to be frame-rate independent.
  - Call each frame to progress game state.

- `void render()`
  - Triggers the render pipeline (`rPipeline->renderAll()`).

- `void clean()`
  - Tears down engine subsystems and SDL. Call on exit.

- `bool running()`
  - Convenience inline that returns engine's running state.

- `SDL_Window* getWindow()`
  - Returns the SDL window pointer for platform-specific integrations.



Public fields you may use (beware direct mutations):
- `objManager* objMgr` — object manager; use its factory method to instantiate objects.
- `mListener* mLnr` — mouse listener (register handlers or poll)
- `kListener* kLnr` — keyboard listener
- `renderPipeline* rPipeline` — rendering pipeline (low-level)

Note: Prefer using the provided wrappers in `game/engine_api.h` (Instantiate, LoadTileMap) in scripts — they're thin convenience helpers and avoid repeating engine internals such as `engine->objMgr->instantiate(...)`.

---

## Object system

Files: `engine/obj/*` and `game/assets/objects.json`.

## Scene manager (engine/scene)

- `sceneManager::loadScene(path, baseX=0, baseY=0, baseZ=0)` — loads the named scene file from the configured scene folder (`sceneManager` is created by `Engine` and rooted at `game/demo/scn` by default). Nested `SCENE` references are loaded recursively and attached as children; object IDs belonging to the loaded scene are returned in a `sceneData` structure.

- `sceneManager::unloadScene(path)` — removes all instantiated objects that were recorded when the scene was loaded (uses the internal `loadedScenes` map).

- `sceneManager::saveScene(name)` — writes the scene to the scene folder using the textual format in `docs/scene.md`. Notes:
  - Creates the scene folder if missing.
  - The header uses the scene name without a `.fscn` suffix.
  - Nested `Scene_OBJ` children are written as `SCENE <scene-file>.fscn x y z;` and are not expanded into their children.
  - Object coordinates are written relative to the saved scene root and rounded to integers.

See `docs/scene.md` for syntax details and examples.

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

Location: `game/engine_api.h`

Includes small inline wrappers:

-- `Object* Instantiate(const string &class, const string &subclass, float x, float y, float z)` — script-facing helper that instantiates a prototype; prefer this over calling engine internals.

-- `void UIAddTextAtNDC(const string &text, float ndc_x, float ndc_y, const string &font = "", int pxSize = 24, bool persistent = false)` — Adds a programmatic UI text entry to the GuiLayer at normalized device coordinates (NDC in [-1..1]). Use `persistent=true` to keep the entry until explicitly removed.

-- `Object* InstantiateUIText(const string &text, float ndc_x, float ndc_y, const string &font = "", int size = 24, const string &instName = "")` — Convenience helper that creates a persistent `ui.text` object and returns it for later manipulation. If `instName` is empty a unique `ui_text_<n>` name is generated.

Usage examples

```cpp
// Programmatic transient UI text at NDC (-1..1)
UIAddTextAtNDC("Lives: 3", -0.9f, 0.9f, "demo/fonts/DMSans.ttf", 20);

// Persistent object-backed label that can be updated later
Object* score = InstantiateUIText("Time: 0", -0.9f, 0.8f, "demo/fonts/DMSans.ttf", 24, "score_label");
// later update:
UIText_OBJ* ul = dynamic_cast<UIText_OBJ*>(score);
if (ul) ul->text = "Time: 1";
```

Include `game/main.h` before `game/engine_api.h` to access the global `engine` pointer.

Example usage:

```cpp
// Programmatic transient UI text at NDC (-1..1)
UIAddTextAtNDC("Lives: 3", -0.9f, 0.9f, "demo/fonts/DMSans.ttf", 20);

// Persistent object-backed label that can be updated later
Object* score = InstantiateUIText("Time: 0", -0.9f, 0.8f, "demo/fonts/DMSans.ttf", 24, "score_label");
// later update:
UIText_OBJ* ul = dynamic_cast<UIText_OBJ*>(score);
if (ul) ul->text = "Time: 1";
```


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
