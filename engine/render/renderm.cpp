#include "engine/render/renderm.h"
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // create empty VBO (we will allocate with update)
    globalVBO = new vbo(nullptr, 0);
}

renderPipeline::~renderPipeline() {
    // free raw images
    for (auto &p : rawImages) {
        if (p.second.pixels) stbi_image_free(p.second.pixels);
    }
    rawImages.clear();

    if (globalVBO) {
        delete globalVBO;
        globalVBO = nullptr;
    }

    if (atlasTex) {
        glDeleteTextures(1, &atlasTex);
        atlasTex = 0;
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

// Ensure image is loaded into rawImages (RGBA)
bool renderPipeline::ensureImageLoaded(const std::string& path) {
    if (rawImages.count(path)) return true;

    int w,h,channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data) {
        std::cerr << "renderPipeline: failed to load image: " << path << std::endl;
        return false;
    }
    RawImage ri;
    ri.w = w;
    ri.h = h;
    ri.pixels = data;
    rawImages[path] = ri;
    return true;
}

// Build a simple shelf-packed atlas and upload to GPU
void renderPipeline::buildAtlasFromRawImages() {
    if (atlasBuilt) return;

    const int ATLAS_W = ATLAS_SIZE;
    const int ATLAS_H = ATLAS_SIZE;

    // Gather list of images to pack (rawImages must be filled)
    std::vector<std::pair<std::string, RawImage>> imgs;
    imgs.reserve(rawImages.size());
    for (auto &p : rawImages) imgs.push_back(p);

    // Simple shelf packer: place images left-to-right, when no space -> new row
    int curX = 0;
    int curY = 0;
    int rowH = 0;

    // create atlas pixel buffer RGBA
    size_t atlasBytes = size_t(ATLAS_W) * size_t(ATLAS_H) * 4;
    unsigned char* atlasPixels = (unsigned char*)malloc(atlasBytes);
    if (!atlasPixels) {
        std::cerr << "renderPipeline: failed to allocate atlas buffer\n";
        return;
    }
    // initialize transparent
    memset(atlasPixels, 0, atlasBytes);

    for (auto &p : imgs) {
        const std::string &path = p.first;
        const RawImage &ri = p.second;

        if (ri.w <= 0 || ri.h <= 0) {
            std::cerr << "renderPipeline: invalid image size: " << path << "\n";
            continue;
        }

        // If image doesn't fit in current row, move to next row
        if (curX + ri.w > ATLAS_W) {
            curX = 0;
            curY += rowH;
            rowH = 0;
        }

        // If it doesn't fit vertically -> fail (you can expand atlas or handle)
        if (curY + ri.h > ATLAS_H) {
            std::cerr << "renderPipeline: atlas overflow, image too large or atlas too small: " << path << "\n";
            continue; // skip this image (transparent will show)
        }

        // copy rows into atlasPixels
        for (int row = 0; row < ri.h; ++row) {
            unsigned char* dst = atlasPixels + ( (curY + row) * ATLAS_W + curX ) * 4;
            unsigned char* src = ri.pixels + row * ri.w * 4;
            memcpy(dst, src, ri.w * 4);
        }

        // store UV coords (note: v coordinate flip depending on your texture coordinates convention)
        float u0 = float(curX) / float(ATLAS_W);
        float v0 = float(curY) / float(ATLAS_H);
        float u1 = float(curX + ri.w) / float(ATLAS_W);
        float v1 = float(curY + ri.h) / float(ATLAS_H);

        // store in atlasMap
        atlasMap[path] = SubTexture{u0, v0, u1, v1};

        // advance
        curX += ri.w;
        if (ri.h > rowH) rowH = ri.h;
    }

    // upload to GL
    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // upload
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ATLAS_W, ATLAS_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasPixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // free temporary atlasPixels (rawImages kept until destructor)
    free(atlasPixels);

    atlasBuilt = true;
    std::cout << "renderPipeline: atlas built with " << atlasMap.size() << " entries\n";
}

// If you load textures dynamically, call this to rebuild atlas
void renderPipeline::rebuildAtlas() {
    // remake atlas from current rawImages
    if (atlasTex) {
        glDeleteTextures(1, &atlasTex);
        atlasTex = 0;
    }
    atlasMap.clear();
    atlasBuilt = false;
    buildAtlasFromRawImages();
}

// Append object vertices to world verts using the subtexture UVs and depth
void renderPipeline::appendObjectToVerts(std::vector<float>& verts, const Object* obj, const SubTexture& uv, float zdepth) {
    const int TILE_W = 64;
    const int TILE_H = 64;
    const int OFFSET_X = engine->virt_sx/2;
    const int OFFSET_Y = engine->virt_sy/2;

    float screenXf = (obj->x - obj->y) * (TILE_W/2.0f) + OFFSET_X;
    float screenYf = (obj->x + obj->y) * 10 - obj->z * 42 + OFFSET_Y;

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

// Main renderAll: builds atlas (if needed), builds one big VBO, draws with single texture
void renderPipeline::renderAll() {
    if (!registry || registry->empty()) return;

    // Compute camera rectangle (full screen for now, modify if using camera offset)
    struct CameraRect {
        int x0, y0, x1, y1;
    };
    CameraRect camRect{ 0, 0, engine->virt_sx, engine->virt_sy };

    //  Ensure all textures used by objects are loaded
    for (auto &objPtr : *registry) {
        if (!objPtr) continue;
        const std::string &path = objPtr->texture;
        if(objPtr->id != 0 && objPtr->texture != "") {
        if (!ensureImageLoaded(path) ) {
            std::cerr << "renderPipeline: warning: missing texture: " << path << std::endl;
        }}
    }

    //  Build atlas once
    if (!atlasBuilt) buildAtlasFromRawImages();

    //  Clear GL buffers
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Sort objects in isometric order
    std::vector<Object*> sorted;
    for (auto& obj : *registry) if (obj && obj->id != 0 && obj->texture != "") sorted.push_back(obj.get());
    std::sort(sorted.begin(), sorted.end(), [](Object* a, Object* b){
        if (a->z != b->z) return a->z < b->z;
        if (a->y != b->y) return a->y < b->y;
        return a->x < b->x;
    });

    //  Build worldVerts with culling
    std::vector<float> worldVerts;
    worldVerts.reserve(sorted.size() * 6 * 8);

    auto isObjectOnScreen = [&](const Object* obj) {
        const int TILE_W = 64;
        const int TILE_H = 64;
        const int OFFSET_X = engine->virt_sx / 2;
        const int OFFSET_Y = engine->virt_sy / 2;

        float screenX = (obj->x - obj->y) * (TILE_W / 2.0f) + OFFSET_X;
        float screenY = (obj->x + obj->y) * 10 - obj->z * 42 + OFFSET_Y;

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
        appendObjectToVerts(worldVerts, obj, uv, depth);
    }

    //  Upload to global VBO
    globalVAO.bind();
    globalVBO->bind();
    globalVBO->update(worldVerts.data(), worldVerts.size());

    // Draw
    defaultShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(worldVerts.size() / 8));

    // Swap buffers
    SDL_GL_SwapWindow(engine->getWindow());
}
