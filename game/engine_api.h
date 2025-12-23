#pragma once

#include <string>

#include "engine/enginem.h"
#include "engine/obj/obj.h"
#include "engine/scene/serialise.h"

// Declare the global engine pointer
extern Engine* engine;

// Lightweight script-facing API

static inline Object* Instantiate(
    const std::string& obj_class,
    const std::string& obj_subclass,
    const std::string& name,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f
) {
    if (!engine || !engine->objMgr) return nullptr;
    return engine->objMgr->instantiate(obj_class, obj_subclass, name, x, y, z);
}

static inline sceneData loadScene(
    const std::string& sceneFile,
    int baseX = 0,
    int baseY = 0,
    int baseZ = 0
) {
    if (!engine || !engine->sceneMgr) return sceneData{};
    return engine->sceneMgr->loadScene(sceneFile, baseX, baseY, baseZ);
}
static inline sceneData unloadScene(
    const std::string& sceneFile
) {
    if (!engine || !engine->sceneMgr) return sceneData{};
    return engine->sceneMgr->unloadScene(sceneFile);
}