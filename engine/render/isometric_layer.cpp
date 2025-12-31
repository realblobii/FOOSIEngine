#include "engine/render/isometric_layer.h"
#include "engine/render/renderm.h"
#include <algorithm>
#include <iostream>

IsometricLayer::IsometricLayer(Engine* eng, std::vector<std::unique_ptr<Object>>* reg, int atlasSize)
    : RenderLayer(eng, atlasSize), registry(reg) {}

void IsometricLayer::prepare(renderPipeline* pipeline) {
    // ensure all textures used by objects are loaded (create placeholder if missing)
    if (!registry || registry->empty()) return;
    for (auto &objPtr : *registry) {
        if (!objPtr) continue;
        if (objPtr->id == 0) continue;
        const std::string &path = objPtr->texture; // may be empty
        ensureImageLoaded(path);
    }
    // let base class ensure atlas is built
    RenderLayer::prepare(pipeline);
}

void IsometricLayer::render(renderPipeline* pipeline) {
    if (!registry || registry->empty()) return;

    // Compute camera rectangle (full screen for now, modify if using camera offset)
    struct CameraRect { int x0, y0, x1, y1; };
    CameraRect camRect{ 0, 0, engine->virt_sx, engine->virt_sy };

    // Debug: print a single-frame summary if something looks wrong (helps track missing textures)
    static bool debugPrinted = false;
    if (!debugPrinted) {
        debugPrinted = true;
        std::cout << "IsometricLayer: debug: registry size=" << (registry ? registry->size() : 0)
                  << " atlasEntries=" << atlasMap.size() << " atlasBuilt=" << atlasBuilt << " atlasTex=" << atlasTex << "\n";
        int count = 0;
        for (auto &objPtr : *registry) {
            if (!objPtr) continue;
            if (objPtr->id == 0) continue;
            std::cout << "  obj id=" << objPtr->id << " class=" << objPtr->obj_class << "." << objPtr->obj_subclass
                      << " tex='" << objPtr->texture << "' invis=" << objPtr->invis << "\n";
            if (++count >= 10) break;
        }
        std::cout << "IsometricLayer: atlas entries list:\n";
        for (auto &p : atlasMap) {
            std::cout << "  '" << p.first << "' -> u0=" << p.second.u0 << " v0=" << p.second.v0 << " u1=" << p.second.u1 << " v1=" << p.second.v1 << "\n";
        }
    }

    // Sort objects in isometric order (include objects without texture; they will use a placeholder)
    std::vector<Object*> sorted;
    for (auto& obj : *registry) if (obj && obj->id != 0 && !obj->invis && obj->obj_class != "ui") sorted.push_back(obj.get());
    std::sort(sorted.begin(), sorted.end(), [](Object* a, Object* b){
        if (a->z != b->z) return a->z < b->z;
        if (a->y != b->y) return a->y < b->y;
        return a->x < b->x;
    });

    // Build worldVerts with culling
    std::vector<float> worldVerts;
    worldVerts.reserve(sorted.size() * 6 * 8);

    const int TILE_W = engine->tile_width;
    const int TILE_H = engine->tile_height;
    const int OFFSET_X = engine->virt_sx/2;
    const int OFFSET_Y = engine->virt_sy/2;

    auto isObjectOnScreen = [&](const Object* obj) {
        // account for camera offset if present
        float camX = 0.0f, camY = 0.0f, camZ = 0.0f;
        if (engine && engine->sceneMgr && engine->sceneMgr->isCamera && engine->sceneMgr->camera) {
            camX = engine->sceneMgr->camera->x;
            camY = engine->sceneMgr->camera->y;
            camZ = engine->sceneMgr->camera->z;
        }

        float ox = obj->x - camX;
        float oy = obj->y - camY;
        float oz = obj->z - camZ;

        float screenX = (ox - oy) * (TILE_W / 2.0f) + OFFSET_X;
        // Match the render projection constants so culling uses the same Y projection
        float screenY = (ox + oy) * 10 - oz * 42 + OFFSET_Y;

        int left   = int(screenX);
        int right  = int(screenX + TILE_W);
        int top    = int(screenY);
        int bottom = int(screenY + TILE_H);

        return !(right < camRect.x0 || left > camRect.x1 ||
                 bottom < camRect.y0 || top > camRect.y1);
    };

    int index = 0;
    for (auto* obj : sorted) {
        if (!isObjectOnScreen(obj)) continue; // skip off-screen objects

        SubTexture uv = atlasMap.count(obj->texture) ? atlasMap[obj->texture] : SubTexture{0,0,1,1};
        float depth = -0.000001f * float(index++);
        pipeline->appendObjectToVerts(worldVerts, obj, uv, depth);
    }

    // Upload to global VBO (shared for layers)
    pipeline->globalVAO.bind();
    pipeline->globalVBO->bind();
    pipeline->globalVBO->update(worldVerts.data(), worldVerts.size());

    // Draw
    pipeline->defaultShader.use();
    pipeline->defaultShader.setInt("texture1", 0); // ensure sampler uses texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(worldVerts.size() / 8));
}
