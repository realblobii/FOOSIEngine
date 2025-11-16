#include "engine/enginem.h"
#include "engine/render/renderm.h"  


Engine::Engine() {}
Engine::~Engine() {}

const int TARGET_FPS = 60;
const float TARGET_FRAME_TIME = 1000.0f / TARGET_FPS; 
Uint32 currentTime;

Uint32 lastTime = SDL_GetTicks();

void Engine::Init(const char* title, int w, int h, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN;

        std::cout << "SDL Successfully Initialised!" << std::endl;

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
        if (window) {
            std::cout << "Window Creation Success!" << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
            std::cout << "Renderer Creation Success!" << std::endl;
        }
          
        if( ( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) )
        {
            std::cout << "IMG Init Success!" << std::endl;
        }

        sdl_sx = w;
        sdl_sy = h;

        
        if (!objMgr) {this->objMgr = new objManager("assets/objects.json");}
        if (!rPipeline){this->rPipeline = new renderPipeline(this);}


    } else {
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        isRunning = false;
    }
    isRunning = true;
}


void Engine::handleEvents() {
    SDL_Event event;
while (SDL_PollEvent(&event)) {
    switch (event.type){
        case SDL_QUIT:
            isRunning = false;
            break;
    }
}
}
void Engine::update(float deltaTime) {
    for (auto& obj : objMgr->registry){
        obj->Update(deltaTime);
    }
}
void Engine::getDeltaT(){
    currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f; // seconds
    lastTime = currentTime;
}

void Engine::render() {
    SDL_RenderClear(renderer);

    rPipeline->renderAll();

    SDL_RenderPresent(renderer);

    Uint32 frameTime = SDL_GetTicks() - currentTime;
    if (frameTime < TARGET_FRAME_TIME)
    {
      SDL_Delay(static_cast<Uint32>(TARGET_FRAME_TIME - frameTime));
    }
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
    if (width > 0 && height > 0){
        SDL_Rect src = {0,0,width,height};
    tex->setTransform(x, y, 0.0, nullptr, &src);
    }

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
    Uint32 currentTime = SDL_GetTicks(); 

    if (currentTime - fpsLastTime >= 1000) { 
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
