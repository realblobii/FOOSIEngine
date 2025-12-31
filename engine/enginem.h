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
#include "engine/obj/obj_mgr.h"  
#include "game/main.h"
#include "engine/input/mouse.h"
#include "engine/input/keyboard.h"
#include "engine/scene/serialise.h" 
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cstdint>

#include "engine/coreclass.h"

class renderPipeline;

class Engine {
public:
    Engine();
    ~Engine();

    // Initialize engine using optional JSON config file path (defaults to "foosiecfg.json" in the working dir)
    void Init(const char* cfgPath = "foosiecfg.json");

    // Engine-configurable variables (populated from JSON config)
    std::string window_title = "FOOSIE";
    int virt_sx = 840;
    int virt_sy = 480;
    bool fullscreen = false;

    // Tile/render configuration
    int tile_width = 64;
    int tile_height = 64;

    // Atlas size used by the renderer (square)
    int atlas_size = 2048;

    // Where textual scene files live (folder relative to game/)
    std::string scene_folder = "demo/scn";

    void handleEvents();
    void update();
    void render();
    void clean();
    void printFPS();
    float getDeltaT();
    float getFPS() const;
    
    /* Texture* loadTexture(const std::string& filename, int x = 0, int y = 0,
                         int width = 0, int height = 0);
 */
    bool running() { return isRunning; }
    SDL_Window* getWindow() { return window; }
    
    objManager* objMgr = nullptr; 
    renderPipeline* rPipeline = nullptr; 
    mListener* mLnr = nullptr;
    kListener* kLnr = nullptr;

    // Scene manager for loading/unloading textual scenes
    sceneManager* sceneMgr = nullptr;

    
    int sdl_sx, sdl_sy;
    // logical (virtual) render resolution used for layout and projection (set from config)
    // `virt_sx` and `virt_sy` are declared above as configurable members.




private:
    bool isRunning = false;
    SDL_Window* window = nullptr;
    //std::vector<Texture*> textures; 
    //std::unordered_map<std::string, Texture*> textureCache;  */
    SDL_GLContext glContext;

    // Timing (delta-time / FPS)
    uint64_t lastCounter = 0;
    double perfFreq = 1.0;
    float deltaTime = 0.0f;
    uint64_t fpsTimerStart = 0;
    int fpsCount = 0;
    float fps = 0.0f;
};

#endif // ENGINEM_H
