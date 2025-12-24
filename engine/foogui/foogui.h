#ifndef FOOGUI_H
#define FOOGUI_H

#include <string>
#include <vector>
#include <unordered_set>

#include "engine/render/render_layer.h"
#include "ft2gl.h"

namespace foogui {

class GuiLayer : public RenderLayer {
public:
    GuiLayer(Engine* eng, int atlasSize = 1024);
    virtual ~GuiLayer();

    // Called before rendering each frame; used to load textures and build atlas
    void prepare(renderPipeline* pipeline) override;
    // Render UI elements (stub)
    void render(renderPipeline* pipeline) override;

    // Simple API for the stub: set a font path and add text entries
    void setFont(const std::string& fontPath, int pxSize);
    void addText(const std::string& txt);

private:
    std::vector<std::string> texts;
    std::string fontPath;
    int fontSize = 24; // default size

    // FreeType font handle
    void* fontHandle = nullptr;
    // keys of glyphs already rendered into the layer rawImages
    std::unordered_set<std::string> renderedGlyphs;
    // glyph metrics cache
    std::unordered_map<std::string, GlyphBitmap> glyphCache;

    // Internal helper: generate a unique key for a glyph
    static std::string glyphKey(const std::string &font, int size, unsigned long cp) {
        return font + ":" + std::to_string(size) + ":" + std::to_string(cp);
    }
};

} // namespace foogui

#endif // FOOGUI_H
