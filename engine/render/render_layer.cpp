#include "engine/render/render_layer.h"
#include "engine/render/renderm.h"
#include <iostream>
#include <cstring>
#include "incl/stb_image.h"
#include <GL/gl.h>

RenderLayer::RenderLayer(Engine* eng, int atlasSize)
    : engine(eng), atlasSize(atlasSize) {}

RenderLayer::~RenderLayer() {
    // free raw images
    for (auto &p : rawImages) {
        if (p.second.pixels) stbi_image_free(p.second.pixels);
    }
    rawImages.clear();

    if (atlasTex) {
        glDeleteTextures(1, &atlasTex);
        atlasTex = 0;
    }
}

bool RenderLayer::ensureImageLoaded(const std::string& path) {
    // If already loaded, ok
    if (rawImages.count(path)) return true;

    // Empty path -> create a 1x1 white placeholder
    if (path.empty()) {
        RawImage ri;
        ri.w = 1;
        ri.h = 1;
        ri.pixels = (unsigned char*)malloc(4);
        ri.pixels[0] = 255; ri.pixels[1] = 255; ri.pixels[2] = 255; ri.pixels[3] = 255;
        rawImages[path] = ri;
        return true;
    }

    int w,h,channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data) {
        std::cerr << "RenderLayer: failed to load image: " << path << " -- using placeholder" << std::endl;
        // Insert placeholder under the requested key so lookups by path succeed
        RawImage ri;
        ri.w = 1;
        ri.h = 1;
        ri.pixels = (unsigned char*)malloc(4);
        ri.pixels[0] = 255; ri.pixels[1] = 0; ri.pixels[2] = 255; ri.pixels[3] = 255; // magenta -> obvious placeholder
        rawImages[path] = ri;
        return true; // return true because we can still render with placeholder
    }
    RawImage ri;
    ri.w = w;
    ri.h = h;
    ri.pixels = data;
    rawImages[path] = ri;
    return true;
}

void RenderLayer::buildAtlasFromRawImages() {
    if (atlasBuilt) return;

    const int ATLAS_W = atlasSize;
    const int ATLAS_H = atlasSize;

    // Gather list of images to pack (rawImages must be filled)
    std::vector<std::pair<std::string, RawImage>> imgs;
    imgs.reserve(rawImages.size());
    for (auto &p : rawImages) imgs.push_back(p);

    // Simple shelf packer: place images left-to-right, when no space -> new row
    int curX = 0;
    int curY = 0;
    int rowH = 0;

    // create atlas pixel buffer RGBA
    size_t atlasBytes = size_t(ATLAS_W) * size_t(ATLAS_H) * 4;
    unsigned char* atlasPixels = (unsigned char*)malloc(atlasBytes);
    if (!atlasPixels) {
        std::cerr << "RenderLayer: failed to allocate atlas buffer\n";
        return;
    }
    // initialize transparent
    memset(atlasPixels, 0, atlasBytes);

    for (auto &p : imgs) {
        const std::string &path = p.first;
        const RawImage &ri = p.second;

        if (ri.w <= 0 || ri.h <= 0) {
            std::cerr << "RenderLayer: invalid image size: " << path << "\n";
            continue;
        }

        // Add 1px padding between images to avoid sampling bleed
        const int PAD = 1;
        // If image doesn't fit in current row, move to next row
        if (curX + ri.w + PAD > ATLAS_W) {
            curX = 0;
            curY += rowH + PAD;
            rowH = 0;
        }

        // If it doesn't fit vertically -> fail (you can expand atlas or handle)
        if (curY + ri.h + PAD > ATLAS_H) {
            std::cerr << "RenderLayer: atlas overflow, image too large or atlas too small: " << path << "\n";
            continue; // skip this image (transparent will show)
        }

        // copy rows into atlasPixels
        for (int row = 0; row < ri.h; ++row) {
            unsigned char* dst = atlasPixels + ( (curY + row) * ATLAS_W + curX ) * 4;
            unsigned char* src = ri.pixels + row * ri.w * 4;
            memcpy(dst, src, ri.w * 4);
        }

        // store UV coords (note: v coordinate flip depending on your texture coordinates convention)
        float u0 = float(curX) / float(ATLAS_W);
        float v0 = float(curY) / float(ATLAS_H);
        float u1 = float(curX + ri.w) / float(ATLAS_W);
        float v1 = float(curY + ri.h) / float(ATLAS_H);

        // store in atlasMap
        atlasMap[path] = SubTexture{u0, v0, u1, v1};

        // advance (add padding)
        curX += ri.w + PAD;
        if (ri.h > rowH) rowH = ri.h + PAD;
    }

    // upload to GL
    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    // Use nearest filtering (no mipmaps) for crisp atlas sampling (important for glyphs)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // upload
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ATLAS_W, ATLAS_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasPixels);

    // free temporary atlasPixels (rawImages kept until destructor)
    free(atlasPixels);

    atlasBuilt = true;
    std::cout << "RenderLayer: atlas built with " << atlasMap.size() << " entries\n";
}

void RenderLayer::rebuildAtlas() {
    if (atlasTex) {
        glDeleteTextures(1, &atlasTex);
        atlasTex = 0;
    }
    atlasMap.clear();
    atlasBuilt = false;
    buildAtlasFromRawImages();
}

void RenderLayer::prepare(renderPipeline* pipeline) {
    // default prepare: build atlas from current rawImages (if not already built)
    if (!atlasBuilt) buildAtlasFromRawImages();
}

void RenderLayer::drawVerts(renderPipeline* pipeline, const std::vector<float>& verts, unsigned int tex) {
    // Upload to global VBO (shared across layers)
    pipeline->globalVAO.bind();
    pipeline->globalVBO->bind();
    pipeline->globalVBO->update(verts.data(), verts.size());

    // Draw using pipeline shader and specified texture
    pipeline->defaultShader.use();
    pipeline->defaultShader.setInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(verts.size() / 8));
}

