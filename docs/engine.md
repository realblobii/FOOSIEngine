# Engine — overview and lifecycle

Location: `engine/enginem.h`, `engine/enginem.cpp`

Purpose
- Describe Engine lifecycle, main responsibilities, and the public API you should call from game code.

What it is
- `Engine` is the runtime owner for the window, GL context, input listeners, object manager, and render pipeline.

Usage
- Call `Init()` once at startup, then per-frame call `handleEvents()`, `update()` and `render()`. Call `clean()` on shutdown.

Important functions
- `Init(const char* cfgPath = "foosiecfg.json")` — Boot the engine, create subsystems, set GL attributes, and read configuration from the JSON file in the working directory (see `game/foosiecfg.json` for options). Key config options include `virt_sx`, `virt_sy`, `fullscreen`, `tile_width`, `tile_height`, `atlas_size`, `scene_folder` (folder where `.fscn` scene files live), and `object_files` (array of prototype JSON files to load in addition to the always-loaded `engine/coreclass.json`).
- `handleEvents()` — Polls SDL events and forwards to input listeners. Handles `SDL_QUIT`.
- `update()` — Computes per-frame delta-time and FPS, calls each registry object's `UpdateDelta(float dt)` (the default implementation calls `Update()` to preserve existing behavior), ticks input listeners (hold handlers), and calls the global `Update()` hook (game-provided). Should be called once per frame.

### Delta-time & `UpdateDelta`

- The engine now computes a high-resolution delta time each frame via `SDL_GetPerformanceCounter()` / `SDL_GetPerformanceFrequency()` and exposes it via `Engine::getDeltaT()` (seconds, as `float`). Use this value for frame-rate independent updates (movement, animations, timers).
- `Engine::getFPS()` returns the recent FPS estimate.
- `Object` provides a `virtual void UpdateDelta(float dt)` method that defaults to calling the old `Update()` — existing objects continue to work unchanged. To migrate an object to use delta time, override `UpdateDelta` and use `dt` for movement/accumulators (e.g., `x += speed * dt`).
- The engine clamps large `dt` values (>0.25s) to avoid large jumps after pauses or when resuming from breakpoints.

- `render()` — Runs `rPipeline->renderAll()` to draw the frame.
- `clean()` — Tears down subsystems and quits SDL.

Fields you may use
- `objManager* objMgr` — instantiate objects via `instantiate()` (prefer the wrapper `Instantiate()` in `game/engine_api.h`).
- `mListener* mLnr`, `kListener* kLnr` — input listeners.
- `renderPipeline* rPipeline` — low-level render access.

Notes
- Prefer the lightweight wrappers in `game/engine_api.h` for scripts (no engine changes required).
- `Engine::update()` calls input `tick()`; handlers that want per-frame hold behavior should register with `onKeyHold`/`onMouseHold`.

## Recent additions (Dec 2025)
- Delta-time support: `Engine` now computes a high-resolution `dt` per frame and exposes `getDeltaT()` / `getFPS()`, and objects can implement `UpdateDelta(float dt)` for frame-rate independent updates.
- UI text subsystem: `ui.text` class and GuiLayer programmatic APIs (`UIAddTextAtNDC`, `InstantiateUIText`) provide simple ways to show screen-space text in any font/size/position.
- Scene serialization: Added `UI` operation and square-bracket property blocks so UI elements can be persisted in `.fscn` scene files.
- Resource behavior: fonts are now discovered in `game/demo/fonts` by default; GuiLayer caches font handles and avoids repeated per-frame loads.
- Object instantiation: `ui.*` objects no longer participate in engine texture resolution (they're rendered by GuiLayer instead).
