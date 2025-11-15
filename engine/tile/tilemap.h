#ifndef TILEMAP_H
#define TILEMAP_H

#include <string>
#include <vector>

// Forward declaration to avoid circular dependency
class Engine;

class TileMap {
public:
    TileMap(Engine* engine, const std::string& jsonFile, int tileWidth, int tileHeight);



private:
    Engine* engine;
    int tileWidth;
    int tileHeight;
};

#endif // TILEMAP_H
