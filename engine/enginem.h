#ifndef ENGINEM_H
#define ENGINEM_H



// INCLUDES FOR ENGINE
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
#include "engine/input/mouse.h"
#include "engine/input/keyboard.h"
#include "engine/scene/serialise.h" 
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
    void update();
    void render();
    void clean();
    void printFPS();
    void getDeltaT();
    
    /* Texture* loadTexture(const std::string& filename, int x = 0, int y = 0,
                         int width = 0, int height = 0);
 */
    bool running() { return isRunning; }
    SDL_Window* getWindow() { return window; }

    TileMap* tileMap = nullptr;     
    objManager* objMgr = nullptr; 
    renderPipeline* rPipeline = nullptr; 
    mListener* mLnr = nullptr;
    kListener* kLnr = nullptr;

    // Scene manager for loading/unloading textual scenes
    sceneManager* sceneMgr = nullptr;

    void loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight);

    // Load a tilemap and return the instantiated object ids; offsets are applied to each tile
    std::vector<int> loadTileMapReturnIds(const std::string& jsonFile, int offsetX, int offsetY, int offsetZ);
    
    int sdl_sx, sdl_sy;




private:
    bool isRunning = false;
    SDL_Window* window = nullptr;
    //std::vector<Texture*> textures; 
    //std::unordered_map<std::string, Texture*> textureCache;  */
    SDL_GLContext glContext;
};

#endif // ENGINEM_H
