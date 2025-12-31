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

    // Add transient text in OpenGL NDC coordinates (-1..1)
    void addTextAtNDC(const std::string &txt, float ndc_x, float ndc_y, const std::string &fontPath = "", int pxSize = 24, bool persistent = false);

private:
    struct UIEntry {
        std::string text;
        int screenX = 0;
        int screenY = 0;
        std::string font;
        int size = 24;
        bool persistent = false; // future: support persistent UI entries
    };

    // Programmatic (transient) UI entries for this frame
    std::vector<UIEntry> uiEntries;

    // objects-based UI entries will be discovered from engine->objMgr->registry at prepare()

    std::vector<std::string> texts;
    std::string fontPath;
    int fontSize = 24; // default size

    // FreeType font handle (default discovered font)
    void* fontHandle = nullptr;
    // Cache per-font handles so we don't reload the same font every frame
    std::unordered_map<std::string, void*> fontCache;

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
