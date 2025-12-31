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
#include "engine/render/render_types.h"

// NOTE: RenderLayer and IsometricLayer are defined in separate headers
#include "engine/render/render_layer.h"
#include "engine/render/isometric_layer.h"

class RenderLayer;
class IsometricLayer;

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
    friend class RenderLayer;
    friend class IsometricLayer;

    explicit renderPipeline(Engine* eng);
    ~renderPipeline();

    void renderAll();
    void rainbowTriangle();

    // register external layers
    void addLayer(std::unique_ptr<RenderLayer> layer) { layers.push_back(std::move(layer)); }

    // core OpenGL abstractions
    glTile obj2gl(const Object* obj); // kept for compatibility if needed
    float screenToNDCx(int screenX);
    float screenToNDCy(int screenY);

    // manually rebuild atlas (call if you add sprites after start)
    void rebuildAtlas();

    // convenience: add text to the UI layer (NDC coords)
    void addTextToUI(const std::string& txt, float ndc_x, float ndc_y, const std::string& font = "", int pxSize = 24, bool persistent = false);

private:
    Engine* engine = nullptr;
    Shader defaultShader;

    // single VAO/VBO for the whole world
    vao globalVAO;
    vbo* globalVBO = nullptr;

    // Layer abstraction: each layer may manage its own atlas/images and rendering rules

    // registered render layers
    std::vector<std::unique_ptr<RenderLayer>> layers;

    // objects registry (used by default isometric layer)
    std::vector<std::unique_ptr<Object>>* registry = nullptr;

    // template for a quad (unchanged)
    static const float quadTemplate[6*8];

    // helper functions
    void appendObjectToVerts(std::vector<float>& verts, const Object* obj, const SubTexture& uv, float zdepth);
};

#endif // RENDERM_H
