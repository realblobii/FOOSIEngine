# Game-side helpers (engine_api)

Location: `game/engine_api.h`

This header provides small convenience wrappers intended for game scripts. They do not modify engine internals.

Helpers
-- `Object* Instantiate(const string &class, const string &subclass, float x, float y, float z)`
  - Script-friendly wrapper for instantiating prototypes. Prefer calling this helper from game code instead of accessing engine internals.
  - Returns a raw pointer owned by the engine registry.

-- `void LoadTileMap(const string &jsonFile, int tileWidth, int tileHeight)`
  - Script-friendly wrapper for loading a tile map into the engine.

Usage
- Include `game/main.h` (exposes global `engine`) before including `game/engine_api.h`.
- These functions are header-only and safe to include in game code.
