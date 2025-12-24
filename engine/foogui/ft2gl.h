#ifndef FT2GL_H
#define FT2GL_H

#include <string>
#include <vector>

// Simple struct describing a rendered glyph bitmap
struct GlyphBitmap {
    int w = 0;
    int h = 0;
    int left = 0;   // bearing x
    int top = 0;    // bearing y
    int advance = 0; // advance.x in pixels
    std::vector<unsigned char> pixels; // RGBA (4 bytes per pixel)
};

// A minimal FreeType wrapper interface. Implementations return a packed RGBA
// bitmap for a given codepoint at a requested pixel size.
// Note: these functions are safe no-ops if FreeType is not available on the build platform
// (they will return an empty GlyphBitmap).

// Initialize the FT subsystem (load library). Returns true on success.
bool ft2gl_init();
// Shutdown and free resources
void ft2gl_shutdown();
// Load a TTF font file; returns a font handle (opaque pointer), or nullptr on failure
void* ft2gl_load_font(const std::string& path);
// Free a loaded font
void ft2gl_free_font(void* font);
// Render a unicode codepoint to a GlyphBitmap at pxSize
GlyphBitmap ft2gl_render_glyph(void* font, unsigned long codepoint, int pxSize);

#endif // FT2GL_H
