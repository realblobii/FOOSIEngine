#ifndef TILEMAP_H
#define TILEMAP_H

#include <string>
#include <vector>
#include "engine/tile/tile.h"

// Forward declaration to avoid circular dependency
class Engine;

class TileMap {
public:
    TileMap(Engine* engine, const std::string& jsonFile, int tileWidth, int tileHeight);

    void render(int offsetX = 0, int offsetY = 0);

    void addTile(const std::string& textureFile, int x, int y, int z);

private:
    Engine* engine;
    int tileWidth;
    int tileHeight;
    std::vector<Tile> tiles;
};

#endif // TILEMAP_H
