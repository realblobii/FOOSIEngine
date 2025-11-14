#include "engine/render/renderm.h"
#include <algorithm>
#include <iostream>

renderPipeline::renderPipeline(Engine* eng)
    : engine(eng), registry(&eng->objMgr->registry)
{
}

void renderPipeline::renderAll() {
    if (!registry || registry->empty()) {
        std::cerr << "[renderPipeline] Warning: registry is empty or null\n";
        return;
    }

    // 1. Copy non-null object pointers
    std::vector<Object*> sortedObjects;
    sortedObjects.reserve(registry->size());
    for (const auto& obj : *registry)
        if (obj) sortedObjects.push_back(obj.get());

    // 2. Sort by Z → Y → X (isometric painter’s order)
    std::sort(sortedObjects.begin(), sortedObjects.end(),
        [](const Object* a, const Object* b) {
            if (a->z != b->z) return a->z < b->z;
            if (a->y != b->y) return a->y < b->y;
            return a->x < b->x;
        });

    // 3. Render all objects
    const int TILE_WIDTH  = 64;
    const int TILE_HEIGHT = 64;
    const int OFFSET_X = 400; // optional screen offset
    const int OFFSET_Y = -150;

    for (const auto* obj : sortedObjects) {
        if (!obj) continue;

        // Convert float XYZ → screen coordinates
        float screenXf = (obj->x - obj->y) * (TILE_WIDTH / 2.0f) + OFFSET_X;
        float screenYf = (obj->x + obj->y) * (10) - (obj->z * (42)) + OFFSET_Y;

        // Cast to int for SDL rendering
        int screenX = static_cast<int>(screenXf);
        int screenY = static_cast<int>(screenYf);

        // Load + render texture (cached)
        Texture* tex = engine->loadTexture(obj->texture, screenX, screenY, TILE_WIDTH, TILE_HEIGHT);
        if (!tex) {
            std::cerr << "[renderPipeline] Failed to load texture: "
                      << obj->texture << "\n";
            continue;
        }

        tex->render();

        // Optional debug output
        // std::cout << "Rendered " << obj->obj_class << ":" << obj->obj_subclass
        //           << " at iso(" << obj->x << "," << obj->y << "," << obj->z << ")"
        //           << " → screen(" << screenX << "," << screenY << ")\n";
    }
}
