#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "engine/enginem.h"
#include "render_types.h"

// Forward
class renderPipeline;

class RenderLayer {
public:
    explicit RenderLayer(Engine* eng, int atlasSize = 2048);
    virtual ~RenderLayer();

    virtual void render(renderPipeline* pipeline) = 0;
    // optional prepare step called before rendering (e.g., rebuild atlas)
    virtual void prepare(renderPipeline* pipeline) {}

    virtual void rebuildAtlas();

protected:
    Engine* engine = nullptr;
    unsigned int atlasTex = 0;
    int atlasSize = 2048;
    bool atlasBuilt = false;
    std::unordered_map<std::string, SubTexture> atlasMap; // path -> uv

    struct RawImage {
        int w, h;
        unsigned char* pixels = nullptr; // 4 channels RGBA
    };
    std::unordered_map<std::string, RawImage> rawImages;

    bool ensureImageLoaded(const std::string& path); // loads into rawImages
    void buildAtlasFromRawImages(); // pack & upload atlas; fills atlasMap
};

#endif // RENDER_LAYER_H
