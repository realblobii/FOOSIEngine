#include "engine/enginem.h"
#include "engine/render/renderm.h"  


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
if (!rPipeline){
    this->rPipeline = new renderPipeline(this);
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
    for (auto& obj : objMgr->registry){
        obj->Update();
    }
}
void Engine::render() {
    SDL_RenderClear(renderer);

    // Render TileMap if it exists
    rPipeline->renderAll();
    // Render individual textures
    for (auto tex : textures)
        tex->render();

    SDL_RenderPresent(renderer);
}


Texture* Engine::loadTexture(const std::string& filename, int x, int y, int width, int height) {
    // 1. Check cache first
    auto it = textureCache.find(filename);
    if (it != textureCache.end()) {
        Texture* tex = it->second;
        tex->setTransform(x, y);
        return tex; // reuse cached texture
    }

    // 2. Not found, load new texture
    Texture* tex = new Texture(renderer);
    if (!tex->loadFromFile(filename)) {
        std::cerr << "Failed to load texture: '" << filename << "'\n";
        delete tex;
        return nullptr;
    }

    tex->setTransform(x, y);
    if (width > 0 && height > 0)
        tex->setTransform(x, y, 0.0, nullptr, new SDL_Rect{0, 0, width, height});

    textures.push_back(tex);
    textureCache[filename] = tex; // store in cache
    return tex;
}

void Engine::loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight) {
    if (tileMap) {
        delete tileMap;
    }
    tileMap = new TileMap(this, jsonFile, tileWidth, tileHeight);
}


void Engine::printFPS() {
    fpsFrames++;
    Uint32 currentTime = SDL_GetTicks(); // milliseconds since SDL init

    if (currentTime - fpsLastTime >= 1000) { // 1 second elapsed
        int fps = fpsFrames;
        std::cout << "FPS: " << fps << std::endl;
        fpsFrames = 0;
        fpsLastTime = currentTime;
    }
}


void Engine::clean() {
    for (auto& tex : textures)
    delete tex;
    textures.clear();
    textureCache.clear();


    if (tileMap) {
        delete tileMap;
        tileMap = nullptr;
    }

    if (objMgr) {
        delete objMgr;
        objMgr = nullptr;
    }
    if(rPipeline){
        delete rPipeline;
        rPipeline = nullptr;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Engine Closed Successfully\n";
}
