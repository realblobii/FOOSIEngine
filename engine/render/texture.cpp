#include "engine/render/texture.h"
#include <SDL2/SDL_image.h>

Texture::Texture()
    : texID(0), x(0), y(0), width(32), height(32)
{}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(const std::string& filename) {
    free(); // free previous texture

    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        std::cerr << "Texture error: Could not load " << filename
                  << " | SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    // Generate OpenGL texture
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLenum format = GL_RGBA;
    if (surface->format->BytesPerPixel == 3) format = GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    width = static_cast<float>(surface->w);
    height = static_cast<float>(surface->h);

    SDL_FreeSurface(surface);
    return true;
}

void Texture::setTransform(float x, float y, float w, float h) {
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
}

void Texture::render() const {
    if (!texID) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y + height);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + height);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void Texture::free() {
    if (texID != 0) {
        glDeleteTextures(1, &texID);
        texID = 0;
    }
}
