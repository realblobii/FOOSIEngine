#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <iostream>

class Texture {
public:
    Texture();
    ~Texture();

    // Load a texture from file using SDL_image and convert to OpenGL texture
    bool loadFromFile(const std::string& filename);

    // Set position, size (angle, flip are ignored for now)
    void setTransform(float x, float y, float width = 32.0f, float height = 32.0f);

    // Render the texture
    void render() const;

    // Free OpenGL texture
    void free();

    // Accessors
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

private:
    GLuint texID;      // OpenGL texture ID
    float x, y;        // Screen position
    float width, height; // Size
};

#endif
