#ifndef ENGINEM_H
#define ENGINEM_H

#include <string>
#include <vector>
#include "incl/glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "engine/render/texture.h"
#include "engine/tile/tilemap.h"
#include "engine/obj/obj_mgr.h"  
#include "game/main.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <unordered_map>

class renderPipeline;

class Engine {
public:
    Engine();
    ~Engine();

    void Init(const char* title, int w, int h, bool fullscreen);
    void handleEvents();
    void update(float deltaTime);
    void render();
    void clean();
    void printFPS();
    void getDeltaT();
    
    Texture* loadTexture(const std::string& filename, int x = 0, int y = 0,
                         int width = 0, int height = 0);

    bool running() { return isRunning; }
    SDL_Renderer* getRenderer() { return renderer; }

    TileMap* tileMap = nullptr;     
    objManager* objMgr = nullptr; 
    renderPipeline* rPipeline = nullptr; 

    void loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight);
    
    int sdl_sx, sdl_sy;

    float deltaTime;

private:
    bool isRunning = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<Texture*> textures; 
    std::unordered_map<std::string, Texture*> textureCache; 
    Uint32 fpsLastTime = 0;  // Last time we updated FPS
    int fpsFrames = 0;        // Frames counted since last FPS update
};

#endif // ENGINEM_H
