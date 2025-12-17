# Game scripting usage (brief)

This small guide shows how to use the engine convenience API from game scripts.

Include these headers in your game code (e.g., `game/main.cpp`):

```cpp
#include "game/main.h"        // exposes global `engine`
#include "game/engine_api.h"  // thin wrappers: Instantiate, LoadTileMap
```

Examples:

- Instantiate a player tile at (10,20):

```cpp
Object *p = Instantiate("tile", "pPlayer", 10.0f, 20.0f, 0.0f);
if (p) p->describe();
```

- Load a tilemap:

```cpp
LoadTileMap("assets/map.json", 16, 16);
```

- Register key handlers:

```cpp
engine->kLnr->onKeyHold("w", [player](const kListener::key &k){ player->y += 1.0f; });
```

Notes:
- `Instantiate()` returns a raw pointer owned by the engine's `objMgr` registry — do not delete it yourself.
- Keep prototype data (shared defaults) in `game/assets/objects.json` and per-instance placement in your `Instantiate()` call.

