# Engine — overview and lifecycle

Location: `engine/enginem.h`, `engine/enginem.cpp`

What it is
- `Engine` is the runtime owner for the window, GL context, input listeners, object manager, render pipeline and tilemap.

Important functions
- `Init(const char* title, int w, int h, bool fullscreen)` — Boot the engine, create subsystems, set GL attributes.
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
