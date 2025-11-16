#include "engine/render/texture.h"
#include <iostream>

Texture::Texture(SDL_Renderer* renderer)
    : renderer(renderer),
      texture(nullptr),
      width(0),
      height(0),
      x(0),
      y(0),
      angle(0.0),
      center(nullptr),
      cliprect(nullptr),
      fliptype(SDL_FLIP_NONE)
{}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(const std::string& filename) {
    free(); 

    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        std::cerr << "Texture error: Could not load " << filename
                  << " | SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Texture error: Could not create texture from "
                  << filename << " | SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }

    width = surface->w;
    height = surface->h;
    SDL_FreeSurface(surface);
    return true;
}

void Texture::setTransform(int x, int y, double angle,
                           SDL_Point* center,
                           SDL_Rect* cliprect,
                           SDL_RendererFlip fliptype) {
    this->x = x;
    this->y = y;
    this->angle = angle;
    this->center = center;
    this->cliprect = cliprect;
    this->fliptype = fliptype;
}

void Texture::render(int x, int y, SDL_Rect* clip) const {
    if (!texture) {
        std::cerr << "Warning: Tried to render a null texture!" << std::endl;
        return;
    }

    SDL_Rect dstRect = { (x >= 0 ? x : this->x),
                         (y >= 0 ? y : this->y),
                         width, height };

    const SDL_Rect* srcRect = clip ? clip : cliprect;
    if (srcRect) {
        dstRect.w = srcRect->w;
        dstRect.h = srcRect->h;
    }

    SDL_RenderCopyEx(renderer, texture, srcRect, &dstRect,
                     angle, center, fliptype);
}

void Texture::free() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        width = height = 0;
    }
}
