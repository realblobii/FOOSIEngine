#ifndef RENDERM_H
#define RENDERM_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

#include "engine/obj/obj.h"
#include "engine/enginem.h"
#include "engine/render/glAbstract.h"
#include "incl/learnopengl/shader_s.h"
#include "incl/stb_image.h"

// Simple struct to hold a sub-rect in atlas (UV coords)
struct SubTexture {
    float u0, v0, u1, v1;
};

struct TileBatch {
     std::vector<float> verts; vbo* VBO = nullptr;
      vao* VAO = nullptr;
       Texture* tex = nullptr; bool dirty = true; // marks if verts need GPU update 
       size_t objectCount = 0; // track how many tiles this batch currently has 
       }; 
struct glTile {
    vbo VBO; 
    Texture texture; 
    glTile(vbo&& v, const Texture& t) : VBO(std::move(v)), texture(t) {} };
class renderPipeline {
public:
    explicit renderPipeline(Engine* eng);
    ~renderPipeline();

    void renderAll();
    void rainbowTriangle();

    // core OpenGL abstractions
    glTile obj2gl(const Object* obj); // kept for compatibility if needed
    float screenToNDCx(int screenX);
    float screenToNDCy(int screenY);

    // manually rebuild atlas (call if you add sprites after start)
    void rebuildAtlas();

private:
    Engine* engine = nullptr;
    Shader defaultShader;

    // single VAO/VBO for the whole world
    vao globalVAO;
    vbo* globalVBO = nullptr;

    // atlas GL handle and mapping
    unsigned int atlasTex = 0;
    // atlas size now configurable via Engine::atlas_size
    int atlasSize = 2048;
    std::unordered_map<std::string, SubTexture> atlasMap; // path -> uv

    // loaded raw images (keeps pixel pointers until atlas built)
    struct RawImage {
        int w, h;
        unsigned char* pixels = nullptr; // 4 channels RGBA
    };
    std::unordered_map<std::string, RawImage> rawImages;

    // objects registry
    std::vector<std::unique_ptr<Object>>* registry = nullptr;

    // template for a quad (unchanged)
    static const float quadTemplate[6*8];

    // helper functions
    bool ensureImageLoaded(const std::string& path); // loads into rawImages
    void buildAtlasFromRawImages(); // pack & upload atlas; fills atlasMap
    void appendObjectToVerts(std::vector<float>& verts, const Object* obj, const SubTexture& uv, float zdepth);

    // one-time atlas built flag
    bool atlasBuilt = false;
};

#endif // RENDERM_H
