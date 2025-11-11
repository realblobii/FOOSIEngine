#include "engine/enginem.h"
#include "game/main.h"
#include "engine/tile/tilemap.h"
#include <json/json.h>
#include <iostream>
#include <fstream>


Engine::Engine() {}
Engine::~Engine() {}

void Engine::Init(const char* title, int w, int h, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        int flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN;

        std::cout << "SDL Successfully Initialised!" << std::endl;

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
        if (window) {
            std::cout << "Window Creation Success!" << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
            std::cout << "Renderer Creation Success!" << std::endl;
        }
          
        if( ( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) )
        {
            std::cout << "IMG Init Success!" << std::endl;
        }

        isRunning = true;

        // --- Initialize object manager here ---
        if (!objMgr) {
    this->objMgr = new objManager("assets/objects.json");
}


    } else {
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        isRunning = false;
    }
}


void Engine::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type){
        case SDL_QUIT:
        isRunning = false;
        break;
        default:
        break;
    }
}
void Engine::update() {
    Update();
}
void Engine::render() {
    SDL_RenderClear(renderer);

    // Render TileMap if it exists
    if (tileMap) {
        // Center the grid horizontally and give a top margin
        int screenWidth  = 800;  // or store in Engine if dynamic
        int topMargin    = 50;
        tileMap->render(screenWidth / 2, topMargin);
    }

    // Render individual textures
    for (auto tex : textures)
        tex->render();

    SDL_RenderPresent(renderer);
}


Texture* Engine::loadTexture(const std::string& filename, int x, int y, int width, int height) {
    Texture* tex = new Texture(renderer);

    if (!tex->loadFromFile(filename)) {
        std::cerr << "Failed to load texture: '" << filename << "'\n";
        delete tex;
        return nullptr; // engine handles error, game doesn’t need to check
    }

    // Set transform with optional scaling
    tex->setTransform(x, y);

    // If width and height are specified, override the texture’s natural size
    if (width > 0 && height > 0) {
        SDL_Rect* clip = new SDL_Rect{0, 0, width, height};
        tex->setTransform(x, y, 0.0, nullptr, clip);
        // note: engine is now responsible for deleting this cliprect in clean()
    }

    textures.push_back(tex);
    return tex;
}
void Engine::loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight) {
    if (tileMap) {
        delete tileMap;
    }
    tileMap = new TileMap(this, jsonFile, tileWidth, tileHeight);
}


void Engine::clean() {
    for (auto tex : textures) {
        delete tex;
    }
    textures.clear();

    if (tileMap) {
        delete tileMap;
        tileMap = nullptr;
    }

    if (objMgr) {
        delete objMgr;
        objMgr = nullptr;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Engine Closed Successfully\n";
}
