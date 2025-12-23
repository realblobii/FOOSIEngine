#ifndef ISOMETRIC_LAYER_H
#define ISOMETRIC_LAYER_H

#include "render_layer.h"
#include <vector>
#include <memory>

class Object;

class IsometricLayer : public RenderLayer {
public:
    IsometricLayer(Engine* eng, std::vector<std::unique_ptr<Object>>* registry, int atlasSize = 2048);
    virtual void prepare(renderPipeline* pipeline) override;
    virtual void render(renderPipeline* pipeline) override;
private:
    std::vector<std::unique_ptr<Object>>* registry = nullptr;
};

#endif // ISOMETRIC_LAYER_H
