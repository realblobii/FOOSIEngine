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

private:
    Engine* engine = nullptr; // pointer to parent engine
    std::vector<std::unique_ptr<Object>>* registry = nullptr; // points to objMgr registry
};

#endif // RENDERM_H
