#include "engine/foogui/foogui.h"
#include "ft2gl.h"
#include "engine/render/renderm.h"
#include "engine/obj/ui_text_oclass.h"
#include <iostream>
#include <unordered_map>
#include <cstring>

namespace foogui {

GuiLayer::GuiLayer(Engine* eng, int atlasSize)
    : RenderLayer(eng, atlasSize)
{
}

GuiLayer::~GuiLayer() {
    // free cached fonts
    for (auto &p : fontCache) {
        if (p.second) ft2gl_free_font(p.second);
    }
    fontCache.clear();
    if (fontHandle) { ft2gl_free_font(fontHandle); fontHandle = nullptr; }
    ft2gl_shutdown();
}

void GuiLayer::prepare(renderPipeline* pipeline) {
    // Initialize font subsystem and prefer a local font from ./demo/fonts (game CWD), fallback to addons/foogui/fonts
    if (fontHandle == nullptr) {
        ft2gl_init();
        fontHandle = ft2gl_load_font(fontPath);
        if (!fontHandle) {
            std::cerr << "GuiLayer: no usable local fonts found in ./demo/fonts or addons/foogui/fonts" << std::endl;
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
    // Render UI entries (textures and text). Text objects (subclass="text") are handled
    // by the text renderer; other UI objects render their resolved `texture` as a UI quad.

    // Ensure FreeType + fonts are available for text rendering
    if (!fontHandle) return;

    // Ensure texture images for non-text UI objects are present in the layer's rawImages
    if (engine && engine->objMgr) {
        for (auto &p : engine->objMgr->registry) {
            if (!p) continue;
            if (p->obj_class != "ui") continue;
            if (p->obj_subclass == "text") continue; // text handled separately
            if (!p->texture.empty()) ensureImageLoaded(p->texture);
        }
    }

    // Helper lambda: ensure glyphs exist for a given font/size/text and queue raw images
    auto ensureGlyphsFor = [&](const std::string &font, int size, const std::string &text) -> bool {
        bool addedGlyphs = false;

        // Quick scan: determine if any glyph is missing (avoid loading font unnecessarily)
        std::string keyFont = font.empty() ? (fontPath.empty() ? std::string("<default>") : fontPath) : font;
        std::vector<unsigned long> missing;
        for (unsigned char c : text) {
            unsigned long cp = static_cast<unsigned long>(c);
            std::string key = glyphKey(keyFont, size, cp);
            if (!renderedGlyphs.count(key)) missing.push_back(cp);
        }
        if (missing.empty()) return false; // nothing to do

        // initialize FreeType if needed
        if (!ft2gl_init()) return false;

        // obtain a font handle, prefer cached handle
        void* fh = nullptr;
        if (!font.empty()) {
            auto it = fontCache.find(font);
            if (it != fontCache.end()) fh = it->second;
            else {
                fh = ft2gl_load_font(font);
                if (fh) fontCache[font] = fh;
                else std::cerr << "GuiLayer: unable to load specified font: " << font << std::endl;
            }
        }

        if (!fh) {
            // ensure default font is loaded (fontHandle) and use it
            if (!fontHandle) {
                fontHandle = ft2gl_load_font(fontPath);
                if (!fontHandle) fontHandle = ft2gl_load_font("");
                if (!fontHandle) {
                    std::cerr << "GuiLayer: no default font available" << std::endl;
                    return false;
                }
            }
            fh = fontHandle;
        }

        // render only missing glyphs
        for (unsigned long cp : missing) {
            std::string key = glyphKey(keyFont, size, cp);
            if (renderedGlyphs.count(key)) continue; // race check
            GlyphBitmap glyph = ft2gl_render_glyph(fh, cp, size);
            renderedGlyphs.insert(key);
            if (glyph.w <= 0 || glyph.h <= 0) continue;
            RawImage ri;
            ri.w = glyph.w; ri.h = glyph.h;
            ri.pixels = (unsigned char*)malloc(size_t(ri.w) * size_t(ri.h) * 4);
            memcpy(ri.pixels, glyph.pixels.data(), size_t(ri.w) * size_t(ri.h) * 4);
            glyphCache[key] = glyph;
            rawImages[key] = ri;
            addedGlyphs = true;
        }

        return addedGlyphs;
    };

    bool addedAny = false;

    // 1) programmatic transient entries
    for (const auto &e : uiEntries) {
        // prefer entry.font, then layer fontPath
        std::string useFont = e.font.empty() ? fontPath : e.font;
        if (ensureGlyphsFor(useFont, e.size, e.text)) addedAny = true;
    }

    // 2) objects from the registry of type ui.text
    if (engine && engine->objMgr) {
        for (auto &p : engine->objMgr->registry) {
            if (!p) continue;
            if (p->obj_class != "ui") continue;
            if (p->obj_subclass != "text") continue;
            // safe cast (we registered UIText_OBJ earlier)
            // Note: include header at compile time; use dynamic_cast to be safe
            // but we only need properties stored on the Object itself via registered setters
            // attempt to read typical properties via known member names (downcast if available)

            // Use RTTI-like approach: try dynamic_cast to access typed members
            // If cast fails (unlikely), fallback to generic properties via texture/text fields
            UIText_OBJ* u = dynamic_cast<UIText_OBJ*>(p.get());
            std::string txt = u ? u->text : std::string();
            std::string useFont = (u && !u->font.empty()) ? u->font : fontPath;
            int useSize = u ? u->size : fontSize;
            // compute screenX/screenY using normalized or pixel props
            int screenX = 0, screenY = 0;
            if (u && u->nx >= 0.0f && u->ny >= 0.0f) {
                screenX = int(u->nx * float(engine->virt_sx));
                screenY = int(u->ny * float(engine->virt_sy));
            } else if (u && (u->sx != 0.0f || u->sy != 0.0f)) {
                screenX = int(u->sx);
                screenY = int(u->sy);
            } else {
                // fallback to object's local coordinates if set
                screenX = int(p->lx);
                screenY = int(p->ly);
            }

            if (ensureGlyphsFor(useFont, useSize, txt)) addedAny = true;
        }
    }

    if (addedAny) {
        std::cout << "GuiLayer: added glyphs at render time, rebuilding atlas\n";
        RenderLayer::rebuildAtlas();
    } else {
        RenderLayer::prepare(pipeline);
    }

    // Build vertices for all UI entries: first images (non-text UI objects), then text
    std::vector<float> verts;

    auto appendImageVerts = [&](const std::string &texPath, int startX, int startY, float overrideW = 0.0f, float overrideH = 0.0f) {
        if (!atlasMap.count(texPath)) { if (rawImages.count(texPath)) std::cerr << "GuiLayer: missing atlas entry for texPath=" << texPath << "\n"; return; }
        SubTexture uv = atlasMap[texPath];
        auto &ri = rawImages[texPath];
        int iw = ri.w; int ih = ri.h;
        if (overrideW > 0.0f) iw = int(overrideW);
        if (overrideH > 0.0f) ih = int(overrideH);
        int x0px = startX;
        int y0px = startY;
        int x1px = x0px + iw;
        int y1px = y0px + ih;
        float z = -0.00002f;
        float x0 = pipeline->screenToNDCx(x0px);
        float y0 = pipeline->screenToNDCy(y0px);
        float x1 = pipeline->screenToNDCx(x1px);
        float y1 = pipeline->screenToNDCy(y1px);

        verts.push_back(x0); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v0);
        verts.push_back(x1); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v0);
        verts.push_back(x1); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v1);

        verts.push_back(x1); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v1);
        verts.push_back(x0); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v1);
        verts.push_back(x0); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v0);
    };

    auto appendTextVerts = [&](const std::string &font, int size, const std::string &text, int startX, int startY) {
        int x = startX;
        int baseline = startY + size; // baseline below top by font size
        for (unsigned char c : text) {
            unsigned long cp = static_cast<unsigned long>(c);
            std::string key = glyphKey(font, size, cp);
            if (!atlasMap.count(key)) { if (rawImages.count(key)) std::cerr << "GuiLayer: missing atlas entry for glyph key=" << key << "\n"; continue; }
            SubTexture uv = atlasMap[key];
            auto &ri = rawImages[key];
            int gw = ri.w, gh = ri.h;
            GlyphBitmap g = glyphCache[key];

            int x0px = x + g.left;
            int y0px = baseline - g.top;
            int x1px = x0px + gw;
            int y1px = y0px + gh;
            float z = -0.00001f;

            float x0 = pipeline->screenToNDCx(x0px);
            float y0 = pipeline->screenToNDCy(y0px);
            float x1 = pipeline->screenToNDCx(x1px);
            float y1 = pipeline->screenToNDCy(y1px);

            verts.push_back(x0); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v0);
            verts.push_back(x1); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v0);
            verts.push_back(x1); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v1);

            verts.push_back(x1); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u1); verts.push_back(uv.v1);
            verts.push_back(x0); verts.push_back(y1); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v1);
            verts.push_back(x0); verts.push_back(y0); verts.push_back(z); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(uv.u0); verts.push_back(uv.v0);

            if (g.advance > 0) x += g.advance; else x += gw + 2;
        }
    };

    // Programmatic entries
    for (const auto &e : uiEntries) {
        std::string useFont = e.font.empty() ? fontPath : e.font;
        appendTextVerts(useFont, e.size, e.text, e.screenX, e.screenY);
    }

    // Scene objects: first add image-based UI objects (non-text)
    if (engine && engine->objMgr) {
        for (auto &p : engine->objMgr->registry) {
            if (!p) continue;
            if (p->obj_class != "ui") continue;
            if (p->obj_subclass == "text") continue; // handled by text pass

            int screenX = 0, screenY = 0;
            // UIBase properties are optional; use normalized coords if present
            // Try dynamic cast to UIBase_OBJ to read w/h if present
            auto *uiBase = dynamic_cast<UIBase_OBJ*>(p.get());
            if (uiBase && uiBase->nx >= 0.0f && uiBase->ny >= 0.0f) {
                screenX = int(uiBase->nx * float(engine->virt_sx));
                screenY = int(uiBase->ny * float(engine->virt_sy));
            } else if (uiBase && (uiBase->sx != 0.0f || uiBase->sy != 0.0f)) {
                screenX = int(uiBase->sx);
                screenY = int(uiBase->sy);
            } else {
                screenX = int(p->lx);
                screenY = int(p->ly);
            }

            float overrideW = uiBase ? uiBase->w : 0.0f;
            float overrideH = uiBase ? uiBase->h : 0.0f;

            if (!p->texture.empty()) appendImageVerts(p->texture, screenX, screenY, overrideW, overrideH);
        }
    }

    // Then add text objects
    if (engine && engine->objMgr) {
        for (auto &p : engine->objMgr->registry) {
            if (!p) continue;
            if (p->obj_class != "ui") continue;
            if (p->obj_subclass != "text") continue;
            UIText_OBJ* u = dynamic_cast<UIText_OBJ*>(p.get());
            if (!u) continue;
            std::string txt = u->text;
            std::string useFont = (u && !u->font.empty()) ? u->font : fontPath;
            int useSize = u ? u->size : fontSize;

            int screenX = 0, screenY = 0;
            if (u->nx >= 0.0f && u->ny >= 0.0f) {
                screenX = int(u->nx * float(engine->virt_sx));
                screenY = int(u->ny * float(engine->virt_sy));
            } else if (u->sx != 0.0f || u->sy != 0.0f) {
                screenX = int(u->sx);
                screenY = int(u->sy);
            } else {
                screenX = int(p->lx);
                screenY = int(p->ly);
            }

            appendTextVerts(useFont, useSize, txt, screenX, screenY);
        }
    }

    if (!verts.empty()) {
        int glyphCount = int(verts.size() / (6 * 8));
        // Draw UI on top: disable depth test so UI is always visible
        glDisable(GL_DEPTH_TEST);
        drawVerts(pipeline, verts, atlasTex);
        glEnable(GL_DEPTH_TEST);
    }

    // Clear transient UI entries (remove only non-persistent)
    uiEntries.erase(std::remove_if(uiEntries.begin(), uiEntries.end(), [](const UIEntry &e){ return !e.persistent; }), uiEntries.end());

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

void GuiLayer::addTextAtNDC(const std::string &txt, float ndc_x, float ndc_y, const std::string &fontPath /*= ""*/, int pxSize /*= 24*/, bool persistent /*= false*/) {
    // convert NDC (-1..1) to screen pixel coordinates. Matches inverse of screenToNDCX/Y
    if (!engine) return;
    int sx = int((ndc_x + 1.0f) * 0.5f * float(engine->virt_sx));
    int sy = int((1.0f - ndc_y) * 0.5f * float(engine->virt_sy));
    UIEntry e;
    e.text = txt;
    e.screenX = sx;
    e.screenY = sy;
    e.font = fontPath;
    e.size = pxSize;
    e.persistent = persistent;
    uiEntries.push_back(e);
}

} // namespace foogui
