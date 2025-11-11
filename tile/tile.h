#pragma once
#include <string>
#include "engine/texture.h"

class Tile {
public:
    Tile(Texture* tex, int x, int y, int z);

    void render(int tileWidth, int tileHeight, int offsetX = 0, int offsetY = 0);

    // Accessors
    int getX() const { return x; }
    int getY() const { return y; }
    int getZ() const { return z; }
    Texture* getTexture() const { return texture; }

private:
    Texture* texture;
    int x, y, z; // grid coordinates
};
