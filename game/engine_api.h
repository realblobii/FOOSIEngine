#pragma once

#include "engine/enginem.h"
#include "engine/obj/obj.h"
#include "engine/scene/serialise.h"
#include <string>

// Lightweight script-facing API that wraps Engine methods. This file does
// not modify the engine implementation; it only exposes convenience
// functions for game scripts that include it.

// Instantiate a new object of `obj_class`/`obj_subclass` at (x,y,z).
// Returns a raw pointer to the created Object (owned by engine->objMgr).
static inline Object* Instantiate(const std::string &obj_class,
                                  const std::string &obj_subclass,
                                  float x=0.0f, float y=0.0f, float z=0.0f) {
    if (!engine || !engine->objMgr) return nullptr;
    return engine->objMgr->instantiate(obj_class, obj_subclass, x, y, z);
}

// Load a tile map using the engine helper. Convenience wrapper.
static inline void loadTilemap(const std::string &jsonFile, int tileWidth, int tileHeight) {
    if (!engine) return;
    engine->loadTileMap(jsonFile, tileWidth, tileHeight);
}

// Convenience wrapper that loads a tilemap and returns instantiated object ids (offset applied)
static inline std::vector<int> loadTilemapIds(const std::string &jsonFile, int offsetX, int offsetY, int offsetZ) {
    if (!engine) return {};
    return engine->loadTileMapReturnIds(jsonFile, offsetX, offsetY, offsetZ);
}

static inline sceneData loadScene(const std::string &sceneFile, int baseX=0, int baseY=0, int baseZ=0) {
    if (!engine || !engine->sceneMgr) return sceneData{};
    return engine->sceneMgr->loadScene(sceneFile, baseX, baseY, baseZ);
}
