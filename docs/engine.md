# Engine — overview and lifecycle

Location: `engine/enginem.h`, `engine/enginem.cpp`

What it is
- `Engine` is the runtime owner for the window, GL context, input listeners, object manager, and render pipeline.

Important functions
- `Init(const char* cfgPath = "foosiecfg.json")` — Boot the engine, create subsystems, set GL attributes, and read configuration from the JSON file in the working directory (see `game/foosiecfg.json` for options). Key config options include `virt_sx`, `virt_sy`, `fullscreen`, `tile_width`, `tile_height`, `atlas_size`, `scene_folder` (folder where `.fscn` scene files live), and `object_files` (array of prototype JSON files to load in addition to the always-loaded `engine/coreclass.json`).
- `handleEvents()` — Polls SDL events and forwards to input listeners. Handles `SDL_QUIT`.
- `update()` — Calls each registry object's `Update()`, ticks input listeners (hold handlers), and calls the global `Update()` hook (game-provided). Should be called once per frame.
- `render()` — Runs `rPipeline->renderAll()` to draw the frame.
- `clean()` — Tears down subsystems and quits SDL.

Fields you may use
- `objManager* objMgr` — instantiate objects via `instantiate()` (prefer the wrapper `Instantiate()` in `game/engine_api.h`).
- `mListener* mLnr`, `kListener* kLnr` — input listeners.
- `renderPipeline* rPipeline` — low-level render access.

Notes
- Prefer the lightweight wrappers in `game/engine_api.h` for scripts (no engine changes required).
- `Engine::update()` calls input `tick()`; handlers that want per-frame hold behavior should register with `onKeyHold`/`onMouseHold`.
