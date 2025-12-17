# Input listeners — mouse & keyboard

Location: `engine/input/mouse.h|cpp`, `engine/input/keyboard.h|cpp`

Modes
- Broadcast: `addHandler(fn)` receives every event copy.
- High-level: `onMouseDown("left", fn)`, `onMouseHold("left", fn)`, `onMouseUp("left", fn)` and similarly for keys: `onKeyDown("a", fn)`, etc.

Hold semantics
- Hold handlers are invoked from `Engine::update()` via `listener->tick()` and will be called each frame while the input is down. Use for continuous movement.

Polling
- `get()` returns-and-clears the last event (single-consumer semantics).

Examples

```cpp
// Run each frame while W is held
engine->kLnr->onKeyHold("w", [player](const kListener::key &k){ player->y += 1.0f; });

// Mouse left click
engine->mLnr->onMouseDown("left", [](const mListener::click& c){ /* one-time */ });
```

Implementation notes
- String names map to SDL constants (`"left"` -> `SDL_BUTTON_LEFT`, `"space"` -> `SDLK_SPACE`).
- Callbacks run synchronously on the main thread (the SDL poll loop). Avoid long-running handlers.
