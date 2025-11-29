#ifndef RENDERM_H
#define RENDERM_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include "engine/obj/obj.h"
#include "engine/enginem.h"
#include "engine/render/texture.h"
#include "engine/render/glAbstract.h"
#include "incl/learnopengl/shader_s.h"

// Simple struct to hold GL objects for a tile batch
struct TileBatch {
    std::vector<float> verts;
    vbo* VBO = nullptr;
    vao* VAO = nullptr;
    Texture* tex = nullptr;
    bool dirty = true;           // marks if verts need GPU update
    size_t objectCount = 0;      // track how many tiles this batch currently has
};


struct glTile {
    vbo VBO;
    Texture texture;

    glTile(vbo&& v, const Texture& t)
        : VBO(std::move(v)), texture(t) {}
};


class renderPipeline {
public:
    explicit renderPipeline(Engine* eng);
    ~renderPipeline();

    void renderAll();
    void rainbowTriangle();

    // core OpenGL abstractions
    void draw(vbo& VBO, Texture& tex);
    glTile obj2gl(const Object* obj);
    void batchTile(TileBatch& batch, const Object* obj);

    float screenToNDCx(int screenX);
    float screenToNDCy(int screenY);

private:
    Engine* engine = nullptr;
    Shader defaultShader;

    vao dVAO;
    Texture dTex;

    std::vector<std::unique_ptr<Object>>* registry = nullptr;

    // For tile batching: map texture path → batch
    std::unordered_map<std::string, TileBatch> tileBatches;

    // Loaded textures cache
    std::unordered_map<std::string, std::unique_ptr<Texture>> loadedTextures;

    static const float quadTemplate[6*8];
};

#endif
