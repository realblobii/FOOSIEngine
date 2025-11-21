#ifndef RENDERM_H
#define RENDERM_H


#include <vector>
#include <memory>
#include "engine/obj/obj.h"
#include "engine/enginem.h"

class renderPipeline {
public:
    explicit renderPipeline(Engine* eng);
    void renderAll();
    void initTests();
    void rainbowTriangle();
    unsigned int VBO,VAO,shaderProgram;

private:
    Engine* engine = nullptr; 
    std::vector<std::unique_ptr<Object>>* registry = nullptr; 
    unsigned int texture;
};

#endif 
