#include "engine/tile/tilemap.h"
#include "engine/obj/obj.h"
#include "engine/tile/tile_oclass.h"
#include "engine/enginem.h"   // full Engine definition needed
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <algorithm>  // for std::sort

TileMap::TileMap(Engine* eng, const std::string& tilemapFile, int tileWidth, int tileHeight)
    : engine(eng), tileWidth(tileWidth), tileHeight(tileHeight)
{
    std::ifstream file(tilemapFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << tilemapFile << "\n";
        return;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;

    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        std::cerr << "Failed to parse JSON: " << errs << "\n";
        return;
    }

    const Json::Value tileArray = root["tiles"];
    for (const auto& t : tileArray) {
        std::string obj_subclass = t["obj_subclass"].asString();
        int x = t["x"].asInt();
        int y = t["y"].asInt();
        int z = t["z"].asInt();

        // Always assume class is "tile"
        auto obj = engine->objMgr->obj_load("tile", obj_subclass);
        if (!obj) {
            std::cerr << "Failed to load object for subclass: " << obj_subclass << "\n";
            continue;
        }

        // Cast to Tile_OBJ to access texture
        Tile_OBJ* tileObj = dynamic_cast<Tile_OBJ*>(obj.get());
        if (!tileObj) {
            std::cerr << "Loaded object is not a Tile_OBJ: " << obj_subclass << "\n";
            continue;
        }

        // Load texture from Tile_OBJ
        engine->objMgr->instantiate("tile", obj_subclass, x, y, z);
    }
}

