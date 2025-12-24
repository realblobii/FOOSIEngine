#include "engine/foogui/foogui.h"
#include "ft2gl.h"
#include "engine/render/renderm.h"
#include <iostream>
#include <unordered_map>
#include <cstring>

namespace foogui {

GuiLayer::GuiLayer(Engine* eng, int atlasSize)
    : RenderLayer(eng, atlasSize)
{
}

GuiLayer::~GuiLayer() {
    if (fontHandle) ft2gl_free_font(fontHandle);
    ft2gl_shutdown();
}

void GuiLayer::prepare(renderPipeline* pipeline) {
    // Initialize font subsystem and ensure we have a local font from addons/foogui/fonts
    if (fontHandle == nullptr) {
        ft2gl_init();
        fontHandle = ft2gl_load_font(fontPath);
        if (!fontHandle) {
            std::cerr << "GuiLayer: no usable local fonts found in addons/foogui/fonts" << std::endl;
        }
    }

    // Render glyphs for any text entries and insert them into this layer's rawImages
    for (const auto &txt : texts) {
        for (char c : txt) {
            unsigned long cp = static_cast<unsigned long>(static_cast<unsigned char>(c));
            std::string key = glyphKey(fontPath, fontSize, cp);
            if (renderedGlyphs.count(key)) continue; // already done

            GlyphBitmap glyph = ft2gl_render_glyph(fontHandle, cp, fontSize);
            if (glyph.w <= 0 || glyph.h <= 0) {
                // nothing to add
                renderedGlyphs.insert(key);
                continue;
            }

            // allocate RawImage and copy glyph pixels (RGBA)
            RawImage ri;
            ri.w = glyph.w;
            ri.h = glyph.h;
            ri.pixels = (unsigned char*)malloc(size_t(ri.w) * size_t(ri.h) * 4);
            memcpy(ri.pixels, glyph.pixels.data(), size_t(ri.w) * size_t(ri.h) * 4);

            // store glyph metrics for placement
            glyphCache[key] = glyph;

            // Use the key string as the image path so atlas maps contain it
            rawImages[key] = ri;
            renderedGlyphs.insert(key);
        }
    }

    // let base class build the atlas from rawImages if needed
    RenderLayer::prepare(pipeline);
}

void GuiLayer::render(renderPipeline* pipeline) {
    // Draw "Hello, World!" in the top-left corner using font glyphs packed into our atlas
    const std::string msg = "Hello, World!";
    const int marginX = 8; // pixels from left
    const int marginY = 8; // pixels from top

    // Ensure we have a font and glyphs
    if (!fontHandle) return;

    // Generate glyphs for the message if missing
    bool addedGlyphs = false;
    for (unsigned char c : msg) {
        unsigned long cp = static_cast<unsigned long>(c);
        std::string key = glyphKey(fontPath, fontSize, cp);
        if (renderedGlyphs.count(key)) continue;
        GlyphBitmap glyph = ft2gl_render_glyph(fontHandle, cp, fontSize);
        renderedGlyphs.insert(key);
        if (glyph.w <= 0 || glyph.h <= 0) continue;
        RawImage ri;
        ri.w = glyph.w; ri.h = glyph.h;
        ri.pixels = (unsigned char*)malloc(size_t(ri.w) * size_t(ri.h) * 4);
        memcpy(ri.pixels, glyph.pixels.data(), size_t(ri.w) * size_t(ri.h) * 4);

        // store glyph metrics and raw image
        glyphCache[key] = glyph;
        rawImages[key] = ri;
        addedGlyphs = true;
    }

    // If we added any glyphs at render time, rebuild the atlas so they're available
    if (addedGlyphs) {
        std::cout << "GuiLayer: added glyphs at render time, rebuilding atlas\n";
        RenderLayer::rebuildAtlas();
    } else {
        // Ensure atlas is built from existing rawImages
        RenderLayer::prepare(pipeline);
    }

    // Build vertices for each glyph and draw
    std::vector<float> verts;
    int x = marginX;
    int y = marginY;
    for (unsigned char c : msg) {
        unsigned long cp = static_cast<unsigned long>(c);
        std::string key = glyphKey(fontPath, fontSize, cp);
        if (!atlasMap.count(key)) { 
            // if we don't even have raw image for this glyph (e.g., space), that's expected — skip silently
            if (rawImages.count(key)) std::cerr << "GuiLayer: missing atlas entry for glyph key=" << key << "\n"; 
            continue; 
        }
        SubTexture uv = atlasMap[key];
        auto &ri = rawImages[key];
        int gw = ri.w;
        int gh = ri.h;
        GlyphBitmap g = glyphCache[key];

        // compute pixel positions using glyph bearings/advance
        int x0px = x + g.left;
        int baseline = marginY + fontSize; // simple baseline below top margin
        int y0px = baseline - g.top; // top coordinate in pixels
        int x1px = x0px + gw;
        int y1px = y0px + gh;
        float z = -0.00001f; // on top

        // compute NDC corners
        float x0 = pipeline->screenToNDCx(x0px);
        float y0 = pipeline->screenToNDCy(y0px);
        float x1 = pipeline->screenToNDCx(x1px);
        float y1 = pipeline->screenToNDCy(y1px);
        // Triangle 1: TL, TR, BR
        verts.push_back(x0); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v0);
        verts.push_back(x1); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v0);
        verts.push_back(x1); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v1);
        // Triangle 2: BR, BL, TL
        verts.push_back(x1); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v1);
        verts.push_back(x0); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v1);
        verts.push_back(x0); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v0);

        // advance by glyph advance (fallback to width + spacing)
        if (g.advance > 0) x += g.advance; else x += gw + 2; // advance with small spacing
    }

    if (!verts.empty()) {
        drawVerts(pipeline, verts, atlasTex);
    }
}

void GuiLayer::setFont(const std::string& path, int pxSize) {
    fontPath = path;
    fontSize = pxSize;
    // reset previously rendered glyphs (font changed)
    renderedGlyphs.clear();
    rawImages.clear();
    if (fontHandle) { ft2gl_free_font(fontHandle); fontHandle = nullptr; }
}

void GuiLayer::addText(const std::string& txt) {
    texts.push_back(txt);
}

} // namespace foogui
