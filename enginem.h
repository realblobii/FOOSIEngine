#ifndef ENGINEM_H
#define ENGINEM_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "engine/texture.h"
#include "engine/tile/tilemap.h"
#include "engine/obj/obj_load.h"  // Add objManager definition

class Engine {
public:
    Engine();
    ~Engine();

    void Init(const char* title, int w, int h, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    Texture* loadTexture(const std::string& filename, int x = 0, int y = 0,
                     int width = 0, int height = 0);

    bool running(){return isRunning;}
    SDL_Renderer* getRenderer() { return renderer; }

    TileMap* tileMap = nullptr;      // pointer to current TileMap
    objManager* objMgr = nullptr; // pointer to object manager

    // Optional: helper to set TileMap from JSON file
    void loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight);

private:
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::vector<Texture*> textures; 
  
};

#endif // ENGINEM_H
