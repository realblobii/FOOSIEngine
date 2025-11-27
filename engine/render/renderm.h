#ifndef RENDERM_H
#define RENDERM_H


#include <vector>
#include <memory>
#include "engine/obj/obj.h"
#include "engine/enginem.h"
#include "engine/render/texture.h"
#include "incl/learnopengl/shader_s.h"

class renderPipeline {
public:
    explicit renderPipeline(Engine* eng);
    void renderAll();
    void initTests();
    void rainbowTriangle();

private:
    Engine* engine = nullptr; 
    Shader defaultShader;
    Shader* defaultSptr = nullptr;

    vao dVAO;   // <---- ADD THESE
    vbo dVBO;   // <----
    Texture dTex;

    Texture*dTexptr;

    std::vector<std::unique_ptr<Object>>* registry = nullptr; 
    
};

#endif 
