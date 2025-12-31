#include "engine/foogui/ft2gl.h"
#include <iostream>
#include <unistd.h>
#include <limits.h>

// Try to include FreeType headers if available
#ifdef __has_include
#if __has_include(<ft2build.h>)
#include <ft2build.h>
#include FT_FREETYPE_H
#define HAVE_FREETYPE 1
#endif
#endif

#ifdef HAVE_FREETYPE
#include <dirent.h>
#include <sys/stat.h>

struct FTContext {
    FT_Library lib = nullptr;
};

struct FTFont {
    FT_Face face = nullptr;
};

static FTContext *g_ft = nullptr;

bool ft2gl_init() {
    if (g_ft) return true;
    g_ft = new FTContext();
    if (FT_Init_FreeType(&g_ft->lib)) {
        std::cerr << "ft2gl: Failed to init FreeType" << std::endl;
        delete g_ft; g_ft = nullptr;
        return false;
    }
    return true;
}

void ft2gl_shutdown() {
    if (!g_ft) return;
    FT_Done_FreeType(g_ft->lib);
    delete g_ft; g_ft = nullptr;
}

void* ft2gl_load_font(const std::string& path) {
    if (!g_ft) return nullptr;

    auto try_load = [&](const std::string &p) -> FTFont* {
        FTFont* f = new FTFont();
        if (FT_New_Face(g_ft->lib, p.c_str(), 0, &f->face) == 0) {
            return f;
        }
        delete f;
        return nullptr;
    };

    // Search in multiple candidate directories and support absolute/relative paths
#ifdef HAVE_FREETYPE
    // Use POSIX directory iteration to avoid depending on <filesystem>

    // Print current working directory for diagnostics

    // Prefer fonts located in the game/demo/fonts directory when running with CWD=game
    std::vector<std::string> searchDirs = {
        "./demo/fonts", // prefer demo relative path when running from game/
        "./fonts",
        "../addons/foogui/fonts",
        "addons/foogui/fonts",
        "/usr/share/fonts/truetype",
        "/usr/share/fonts"
    };

    auto try_load_path = [&](const std::string &p) -> FTFont* {
        struct stat st;
        if (stat(p.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            FTFont* f = try_load(p);
            if (f) return f;
        }
        return nullptr;
    };

    // If user provided a path (absolute, relative or filename), try sensible candidates
    if (!path.empty()) {
        // Try as absolute path
        if (path.size() > 0 && path[0] == '/') {
            if (FTFont* f = try_load_path(path)) return f;
            std::cerr << "ft2gl: requested absolute path not loadable: " << path << std::endl;
        }
        // Try as-is relative to CWD
        if (FTFont* f = try_load_path(path)) return f;
        // Try under each search dir
        for (const auto &dpath : searchDirs) {
            std::string candidate = dpath + "/" + path;
            if (FTFont* f = try_load_path(candidate)) return f;
        }
        // Try basename under search dirs
        size_t pos = path.find_last_of("/\\");
        std::string base = (pos == std::string::npos) ? path : path.substr(pos+1);
        for (const auto &dpath : searchDirs) {
            std::string candidate2 = dpath + "/" + base;
            if (FTFont* f = try_load_path(candidate2)) return f;
        }

        std::cerr << "ft2gl: requested font not found for: " << path << std::endl;
    }

    // Fallback: scan candidate directories for .ttf/.otf files
    for (const auto &dpath : searchDirs) {
        DIR *d = opendir(dpath.c_str());
        if (!d) {
            std::cerr << "ft2gl: fonts dir not accessible: " << dpath << std::endl;
            continue;
        }
        std::cerr << "ft2gl: scanning fonts dir: " << dpath << std::endl;
        struct dirent *ent;
        while ((ent = readdir(d)) != nullptr) {
            if (ent->d_type != DT_REG && ent->d_type != DT_UNKNOWN) continue;
            std::string name = ent->d_name;
            // check extension
            size_t dot = name.find_last_of('.');
            if (dot == std::string::npos) continue;
            std::string ext = name.substr(dot);
            if (ext == ".ttf" || ext == ".otf" || ext == ".TTF" || ext == ".OTF") {
                std::string full = dpath + "/" + name;
                FTFont* f = try_load(full);
                if (f) { closedir(d); return f; }
            }
        }
        closedir(d);
    }

#endif

    std::cerr << "ft2gl: no usable fonts found (checked multiple paths)" << std::endl;
    return nullptr;
}

void ft2gl_free_font(void* font) {
    if (!font) return;
    FTFont* f = (FTFont*)font;
    if (f->face) FT_Done_Face(f->face);
    delete f;
}

GlyphBitmap ft2gl_render_glyph(void* font, unsigned long codepoint, int pxSize) {
    GlyphBitmap out;
    if (!font) return out;
    FTFont* f = (FTFont*)font;
    FT_Face face = f->face;
    if (!face) return out;

    FT_Set_Pixel_Sizes(face, 0, pxSize);
    if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER)) {
        std::cerr << "ft2gl: failed to load glyph " << codepoint << std::endl;
        return out;
    }

    FT_GlyphSlot g = face->glyph;
    out.w = g->bitmap.width;
    out.h = g->bitmap.rows;
    out.left = g->bitmap_left;
    out.top = g->bitmap_top;
    out.advance = (g->advance.x >> 6);
    out.pixels.resize(size_t(out.w) * size_t(out.h) * 4);

    // Convert FT grayscale bitmap to RGBA (white text, alpha from FT)
    for (int y = 0; y < out.h; ++y) {
        for (int x = 0; x < out.w; ++x) {
            unsigned char a = g->bitmap.buffer[y * g->bitmap.pitch + x];
            size_t idx = (y * out.w + x) * 4;
            out.pixels[idx + 0] = 255; // R
            out.pixels[idx + 1] = 255; // G
            out.pixels[idx + 2] = 255; // B
            out.pixels[idx + 3] = a;   // A
        }
    }
    return out;
}

#else // HAVE_FREETYPE

static bool s_dummy_init = false;

bool ft2gl_init() {
    s_dummy_init = true;
    std::cerr << "ft2gl: FreeType not available; text rendering disabled" << std::endl;
    return false;
}
void ft2gl_shutdown() { s_dummy_init = false; }
void* ft2gl_load_font(const std::string& path) { (void)path; return nullptr; }
void ft2gl_free_font(void* font) { (void)font; }
GlyphBitmap ft2gl_render_glyph(void* font, unsigned long codepoint, int pxSize) { (void)font;(void)codepoint;(void)pxSize; return GlyphBitmap(); }

#endif
