#ifndef ENGINEM_H
#define ENGINEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>

#include "engine/render/texture.h"
#include "engine/render/renderm.h"
#include "engine/tile/tilemap.h"
#include "engine/obj/obj_mgr.h"

class Engine {
public:
    Engine();
    ~Engine();

    // Initialization
    void Init(const char* title, int width, int height, bool fullscreen);

    // Main loop
    void handleEvents();
    void update();
    void render();

    // Texture management
    Texture* loadTexture(const std::string& filename,
                         int x = 0, int y = 0,
                         int width = 0, int height = 0);

    // Tilemap
    void loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight);

    // FPS
    void printFPS();

    // Cleanup
    void clean();

    // State
    bool running() const { return isRunning; }

    // Screen size
    int sdl_sx = 0;
    int sdl_sy = 0;

    // Delta time (seconds)

    // Public members
    TileMap* tileMap = nullptr;
    objManager* objMgr = nullptr;
    renderPipeline* rPipeline = nullptr;

private:
    bool isRunning = false;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr; // only needed for legacy Texture load
    SDL_GLContext glContext = nullptr;

    std::vector<Texture*> textures;
    std::unordered_map<std::string, Texture*> textureCache;

    
};

#endif // ENGINEM_H
