#include "engine/render/renderm.h"
#include "engine/render/render_layer.h"
#include "engine/render/isometric_layer.h"
#include "engine/foogui/foogui.h"
#include <algorithm>
#include <cstring> // memcpy

// quadTemplate same as your version (posx,posy,posz, nx,ny,nz, u,v)
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
      defaultShader("shader/default.vs", "shader/default.fs"),
      registry(&eng->objMgr->registry)
{
    // atlas size used by layers
    int layerAtlasSize = engine ? engine->atlas_size : 2048;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Don't abort if there's no Scene Manager; rendering can proceed without a camera
    if (!engine->sceneMgr) {
        std::cerr << "[renderPipeline] Warning: Scene manager not present; rendering may not be fully functional." << std::endl;
    }

    // create empty VBO (we will allocate with update)
    globalVBO = new vbo(nullptr, 0);

    // default: add the existing isometric renderer as one layer
    layers.emplace_back(std::make_unique<IsometricLayer>(engine, registry, layerAtlasSize));
} 

renderPipeline::~renderPipeline() {
    // Layers own their own raw images and textures and will clean up in their destructors
    if (globalVBO) {
        delete globalVBO;
        globalVBO = nullptr;
    }
}

float renderPipeline::screenToNDCx(int screenX) {
    // screenX is in virtual/logical pixels; convert to NDC using virtual width
    return (2.0f * float(screenX) / float(engine->virt_sx)) - 1.0f;
}
float renderPipeline::screenToNDCy(int screenY) {
    // screenY is in virtual/logical pixels; convert to NDC using virtual height
    return 1.0f - (2.0f * float(screenY) / float(engine->virt_sy));
}

void renderPipeline::addTextToUI(const std::string& txt, float ndc_x, float ndc_y, const std::string& font, int pxSize, bool persistent) {
    // forward to first GuiLayer instance (if any)
    for (auto &layer : layers) {
        if (!layer) continue;
        auto g = dynamic_cast<foogui::GuiLayer*>(layer.get());
        if (g) { g->addTextAtNDC(txt, ndc_x, ndc_y, font, pxSize, persistent); return; }
    }
    std::cerr << "[renderPipeline] Warning: no GuiLayer found to add UI text" << std::endl;
}


// Append object vertices to world verts using the subtexture UVs and depth
void renderPipeline::appendObjectToVerts(std::vector<float>& verts, const Object* obj, const SubTexture& uv, float zdepth) {
    const int TILE_W = engine->tile_width;
    const int TILE_H = engine->tile_height;
    const int OFFSET_X = engine->virt_sx/2;
    const int OFFSET_Y = engine->virt_sy/2;

    // If a camera exists in the scene manager, offset world coordinates by it, otherwise use origin
    float camX = 0.0f, camY = 0.0f, camZ = 0.0f;
    if (engine && engine->sceneMgr && engine->sceneMgr->isCamera && engine->sceneMgr->camera) {
        camX = engine->sceneMgr->camera->x;
        camY = engine->sceneMgr->camera->y;
        camZ = engine->sceneMgr->camera->z;
    }

    float objX = obj->x - camX;
    float objY = obj->y - camY;
    float objZ = obj->z - camZ;

    float screenXf = (objX - objY) * (TILE_W/2.0f) + OFFSET_X;
    // Use the same projection math as the culling routine so culling matches rendering
    float screenYf = (objX + objY) * (TILE_W / 6.4f) - (objZ * (TILE_H * 0.66f)) + OFFSET_Y;

    int baseX = int(screenXf);
    int baseY = int(screenYf);

    for (int i = 0; i < 6; ++i) {
        const float* v = &quadTemplate[i*8];
        float px = baseX + v[0];
        float py = baseY + v[1];

        verts.push_back(screenToNDCx(int(px)));
        verts.push_back(screenToNDCy(int(py)));
        verts.push_back(zdepth);
        verts.push_back(v[3]); // nx
        verts.push_back(v[4]); // ny
        verts.push_back(v[5]); // nz

        // remap uv from [0..1] to atlas uv
        float su = uv.u0 + v[6] * (uv.u1 - uv.u0);
        float sv = uv.v0 + v[7] * (uv.v1 - uv.v0);
        verts.push_back(su);
        verts.push_back(sv);
    }
}

// Main renderAll: dispatches to registered layers and performs final buffer swap once
void renderPipeline::renderAll() {
    if (!registry || registry->empty()) return;

    // Clear GL buffers once per frame
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Prepare and render each layer in order
    for (auto &layer : layers) {
        if (!layer) continue;
        layer->prepare(this);
        layer->render(this);
    }

    // Swap buffers once after all layers rendered
    SDL_GL_SwapWindow(engine->getWindow());
}

// request full atlas rebuild on all layers
void renderPipeline::rebuildAtlas() {
    for (auto &layer : layers) {
        if (layer) layer->rebuildAtlas();
    }
}


