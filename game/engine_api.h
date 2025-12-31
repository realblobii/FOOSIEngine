#pragma once

#include <string>

#include "engine/enginem.h"
#include "engine/render/renderm.h"
#include "engine/obj/obj.h"
#include "engine/obj/ui_text_oclass.h"
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
    float baseX = 0.0f,
    float baseY = 0.0f,
    float baseZ = 0.0f
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

// Add text to the UI layer at normalized device coords (-1..1)
static inline void UIAddTextAtNDC(const std::string& text, float ndc_x, float ndc_y, const std::string& font = "", int pxSize = 24, bool persistent = false) {
    if (!engine || !engine->rPipeline) return;
    engine->rPipeline->addTextToUI(text, ndc_x, ndc_y, font, pxSize, persistent);
}

// Instantiate a persistent ui.text object in the engine registry and return it for later manipulation
// If `instName` is empty, a default name `ui_text_<n>` is generated.
static inline Object* InstantiateUIText(const std::string& text, float ndc_x, float ndc_y, const std::string& font = "", int size = 24, const std::string& instName = "") {
    if (!engine || !engine->objMgr) return nullptr;
    static int ui_counter = 1;
    std::string name = instName;
    if (name.empty()) name = "ui_text_" + std::to_string(ui_counter++);
    Object* obj = engine->objMgr->instantiate("ui", "text", name, 0.0f, 0.0f, 0.0f);
    if (!obj) return nullptr;
    UIText_OBJ* u = dynamic_cast<UIText_OBJ*>(obj);
    if (u) {
        u->text = text;
        u->font = font;
        u->size = size;
        u->nx = (ndc_x + 1.0f) * 0.5f;
        u->ny = (ndc_y + 1.0f) * 0.5f;
    }
    return obj;
}