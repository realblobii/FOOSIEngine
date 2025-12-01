#include "engine/render/renderm.h"
#include <algorithm>
#include <iostream>
#include "incl/stb_image.h"

const float renderPipeline::quadTemplate[6*8] = {
    0,  0, 0,  0,0,1,  1,1,
    64, 0, 0,  0,0,1,  0,1,
    64,64,0,  0,0,1,  0,0,
    64,64,0,  0,0,1,  0,0,
    0, 64,0,  0,0,1,  1,0,
    0,  0,0,  0,0,1,  1,1
};

renderPipeline::renderPipeline(Engine* eng)
    : engine(eng),
      registry(&eng->objMgr->registry),
      defaultShader("shader/default.vs", "shader/default.fs"),
      dTex("assets/grass.png"),
      dVAO()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

renderPipeline::~renderPipeline() {
    for (auto& [path, batch] : tileBatches) {
        delete batch.VBO;
        delete batch.VAO;
        // textures managed by loadedTextures
    }
}

void renderPipeline::draw(vbo& VBO, Texture& tex) {
    tex.bind();
    dVAO.bind();
    VBO.bind();
    glDrawArrays(GL_TRIANGLES, 0, VBO.getCount());
}

float renderPipeline::screenToNDCx(int screenX) {
    return (2.0f * float(screenX) / float(engine->sdl_sx)) - 1.0f;
}
float renderPipeline::screenToNDCy(int screenY) {
    return 1.0f - (2.0f * float(screenY) / float(engine->sdl_sy));
}

// Converts object to a glTile (non-tile objects only)
glTile renderPipeline::obj2gl(const Object* obj) {
    const int TILE_W = 64;
    const int TILE_H = 64;
    const int OFFSET_X = engine->sdl_sx/2;
    const int OFFSET_Y = engine->sdl_sy/2;

    float screenXf = (obj->x - obj->y)*(TILE_W/2.0f)+OFFSET_X;
    float screenYf = (obj->x + obj->y)*10 - obj->z*42 + OFFSET_Y;

    int baseX = int(screenXf);
    int baseY = int(screenYf);

    std::vector<float> verts;
    verts.reserve(6*8);

    for (int i=0;i<6;i++){
        const float* v = &quadTemplate[i*8];
        float px = baseX+v[0];
        float py = baseY+v[1];

        verts.push_back(screenToNDCx(int(px)));
        verts.push_back(screenToNDCy(int(py)));
        verts.push_back(0.0f);
        verts.push_back(v[3]);
        verts.push_back(v[4]);
        verts.push_back(v[5]);
        verts.push_back(v[6]);
        verts.push_back(v[7]);
    }

    Texture* texPtr = nullptr;
    if (loadedTextures.count(obj->texture)==0)
        loadedTextures[obj->texture] = std::make_unique<Texture>(obj->texture);
    texPtr = loadedTextures[obj->texture].get();

    return glTile{ vbo(verts.data(), verts.size()), *texPtr };
}

// Adds a tile's vertices to a batch
void renderPipeline::batchTile(TileBatch& batch, const Object* obj) {
    const int TILE_W = 64;
    const int TILE_H = 64;
    const int OFFSET_X = engine->sdl_sx/2;
    const int OFFSET_Y = engine->sdl_sy/2;

    float screenXf = (obj->x - obj->y)*(TILE_W/2.0f)+OFFSET_X;
    float screenYf = (obj->x + obj->y)*10 - obj->z*42 + OFFSET_Y;

    int baseX = int(screenXf);
    int baseY = int(screenYf);

    for (int i=0;i<6;i++){
        const float* v = &quadTemplate[i*8];
        float px = screenToNDCx(int(baseX+v[0]));
        float py = screenToNDCy(int(baseY+v[1]));

        if (px >= -0.1 && py >= -0.1 && px <= 1.1 && py <= 1.1){
        batch.verts.push_back(px);
        batch.verts.push_back(py);
        batch.verts.push_back(0.0f);
        batch.verts.push_back(v[3]);
        batch.verts.push_back(v[4]);
        batch.verts.push_back(v[5]);
        batch.verts.push_back(v[6]);
        batch.verts.push_back(v[7]);}
    }

    batch.dirty = true;
}

// Main render function
void renderPipeline::renderAll() {
    if (!registry || registry->empty()) return;

    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sort objects
    std::vector<Object*> sorted;
    for (auto& obj : *registry) if (obj) sorted.push_back(obj.get());
    std::sort(sorted.begin(), sorted.end(), [](Object* a, Object* b){
        if (a->z!=b->z) return a->z<b->z;
        if (a->y!=b->y) return a->y<b->y;
        return a->x<b->x;
    });

    // Build batches
    for (auto* obj : sorted) {
            if (tileBatches.count(obj->texture)==0) {
                TileBatch batch;
                // create persistent VBO + VAO
                batch.VAO = new vao();
                batch.VBO = new vbo(batch.verts.data(), batch.verts.size());
                // store texture
                if (loadedTextures.count(obj->texture)==0)
                    loadedTextures[obj->texture] = std::make_unique<Texture>(obj->texture);
                batch.tex = loadedTextures[obj->texture].get();
                tileBatches[obj->texture] = batch;
            }
            batchTile(tileBatches[obj->texture], obj);
        
    }

    // Draw all tile batches
    for (auto& [texPath, batch] : tileBatches) {
        if (batch.verts.empty()) continue;

        defaultShader.use();
        batch.VAO->bind();

        if (batch.dirty) {
            batch.VBO->bind();
            batch.VBO->update(batch.verts.data(), batch.verts.size());
            batch.dirty = false;
        } else {
                
        }

        batch.tex->bind();
        glDrawArrays(GL_TRIANGLES, 0, batch.verts.size()/8);

        // clear verts for next frame
        batch.verts.clear();
    }

    SDL_GL_SwapWindow(engine->getWindow());
}
