// INCLUDES FOR ENGINE
#include "engine/render/renderm.h"
#include <algorithm>
#include <iostream>
#include "incl/stb_image.h"



float testVertices[] = {
    // positions        // colors         // tex coords
     0.5f,  0.5f, 0.0f,  1,0,0,  1.0f, 1.0f, // top right
     0.5f, -0.5f, 0.0f,  0,1,0,  1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,  0,0,1,  0.0f, 0.0f, // bottom left

    -0.5f, -0.5f, 0.0f,  0,0,1,  0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,  1,1,0,  0.0f, 1.0f, // top left
     0.5f,  0.5f, 0.0f,  1,0,0,  1.0f, 1.0f  // top right
};


renderPipeline::renderPipeline(Engine* eng)
    : engine(eng),
      registry(&eng->objMgr->registry),
      defaultShader("shader/default.vs", "shader/default.fs"),
      dTex("assets/grass.png"),
      dVAO(),
      dVBO(testVertices, sizeof(testVertices) / sizeof(float))
{
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    char infoLog[512];
}

void renderPipeline::draw(vbo& VBO, Texture& tex){
    tex.bind();
    dVAO.bind();
    VBO.bind();
    glDrawArrays(GL_TRIANGLES, 0, (sizeof(testVertices) / sizeof(float))/8);
}
void renderPipeline::rainbowTriangle(){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        // draw our first triangle
        defaultShader.use();
        draw(dVBO, dTex);
        

        SDL_GL_SwapWindow(engine->getWindow());
}
/* void renderPipeline::renderAll() {
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
    const int OFFSET_X = engine->sdl_sx/2; 
    const int OFFSET_Y = engine->sdl_sy/2;

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
    }
}
 */