#ifndef RENDERM_H
#define RENDERM_H


#include <vector>
#include <memory>
#include "engine/obj/obj.h"
#include "engine/enginem.h"
#include "engine/render/texture.h"
#include "engine/render/glAbstract.h"
#include "incl/learnopengl/shader_s.h"

class renderPipeline {
public:
    explicit renderPipeline(Engine* eng);
    void renderAll();
    void initTests();
    void rainbowTriangle();
    void freeGlObjects();

private:
    Engine* engine = nullptr; 
    Shader defaultShader;

    vao dVAO;   // <---- ADD THESE
    vbo dVBO;   // <----
    Texture dTex;

    std::vector<std::unique_ptr<Object>>* registry = nullptr; 
    
};

#endif 
