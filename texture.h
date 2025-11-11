#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Texture {
public:
    // Constructor: takes an SDL_Renderer pointer
    explicit Texture(SDL_Renderer* renderer);

    // Destructor: automatically frees texture
    ~Texture();

    // Load an image from file
    bool loadFromFile(const std::string& filename);

    // Set position, rotation, and other transform data
    void setTransform(int x, int y, double angle = 0.0,
                      SDL_Point* center = nullptr,
                      SDL_Rect* cliprect = nullptr,
                      SDL_RendererFlip fliptype = SDL_FLIP_NONE);

    // Render texture (optionally override x/y/clip)
    void render(int x = -1, int y = -1, SDL_Rect* clip = nullptr) const;

    // Free the SDL texture
    void free();

    // Accessors
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    int width, height;
    int x, y;
    double angle;
    SDL_Point* center;
    SDL_Rect* cliprect;
    SDL_RendererFlip fliptype;
};
