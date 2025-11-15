#include "engine/render/renderm.h"
#include "engine/enginem.h" // full definition of Engine

#include <algorithm>
#include <iostream>

renderPipeline::renderPipeline(Engine* eng)
    : engine(eng), registry(&eng->objMgr->registry)
{
}
bool warnedEmpty = false;
void renderPipeline::renderAll() {
    if (!registry || registry->empty()) {
        if (!warnedEmpty) {
            std::cerr << "[renderPipeline] Warning: registry is empty or null\n";
            warnedEmpty = true;
        }
        return;
    }

    std::vector<Object*> sortedObjects;
    sortedObjects.reserve(registry->size());
    for (const auto& obj : *registry)
        if (obj) sortedObjects.push_back(obj.get());

    std::sort(sortedObjects.begin(), sortedObjects.end(),
        [](const Object* a, const Object* b) {
            if (a->z != b->z) return a->z < b->z;
            if (a->y != b->y) return a->y < b->y;
            return a->x < b->x;
        });

    const int TILE_WIDTH  = 64;
    const int TILE_HEIGHT = 64;
    const float OFFSET_X = engine->sdl_sx / 2.0f;
    const float OFFSET_Y = engine->sdl_sy / 2.0f;

    for (const auto* obj : sortedObjects) {
        if (!obj) continue;

        float screenXf = (obj->x - obj->y) * (TILE_WIDTH / 2.0f) + OFFSET_X;
        float screenYf = (obj->x + obj->y) * 10.0f - (obj->z * 42.0f) + OFFSET_Y;

        int screenX = static_cast<int>(screenXf);
        int screenY = static_cast<int>(screenYf);

        // now engine is fully known, this works
        Texture* tex = engine->loadTexture(obj->texture, screenX, screenY, TILE_WIDTH, TILE_HEIGHT);
        if (!tex) {
            std::cerr << "[renderPipeline] Failed to load texture: " << obj->texture << "\n";
            continue;
        }

        tex->render();
    }
}
