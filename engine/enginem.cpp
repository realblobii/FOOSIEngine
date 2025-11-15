#include "engine/enginem.h"
#include "engine/render/renderm.h"  


Engine::Engine() {}
Engine::~Engine() {}


void Engine::Init(const char* title, int w, int h, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN;

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  w, h, flags | SDL_WINDOW_OPENGL);

        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            isRunning = false;
            return;
        }

        // Create OpenGL context
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
            isRunning = false;
            return;
        }

        // Enable V-Sync (optional)
        SDL_GL_SetSwapInterval(1);

        // OpenGL initial settings
        glViewport(0, 0, w, h);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        sdl_sx = w;
        sdl_sy = h;

        // Object manager and render pipeline
        objMgr = new objManager("assets/objects.json");
        rPipeline = new renderPipeline(this);

        isRunning = true;
    } else {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        isRunning = false;
    }
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
void Engine::update() {
    for (auto& obj : objMgr->registry){
        obj->Update();
    }
}


void Engine::render() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render all objects via pipeline
    rPipeline->renderAll();

    // Swap buffers (OpenGL display)
    SDL_GL_SwapWindow(window);
}



Texture* Engine::loadTexture(const std::string& filename, int x, int y, int width, int height) {
    // 1. Check cache first
    auto it = textureCache.find(filename);
    if (it != textureCache.end()) {
        Texture* tex = it->second;
        tex->setTransform(x, y);
        return tex; // reuse cached texture
    }

    // 2. Load new texture
    Texture* tex = new Texture(); // OpenGL version doesn't need SDL_Renderer
    if (!tex->loadFromFile(filename)) { // load into OpenGL texture
        std::cerr << "[Engine::loadTexture] Failed to load texture: '" << filename << "'\n";
        delete tex;
        return nullptr;
    }

    // 3. Set transform (screen coordinates)
    tex->setTransform(x, y, width > 0 ? width : tex->getWidth(), 
                             height > 0 ? height : tex->getHeight());

    // 4. Cache the texture
    textures.push_back(tex);
    textureCache[filename] = tex;

    return tex;
}


void Engine::loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight) {
    if (tileMap) {
        delete tileMap;
    }
    tileMap = new TileMap(this, jsonFile, tileWidth, tileHeight);
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

    // Destroy render pipeline **first**
    if (rPipeline) {
        delete rPipeline;
        rPipeline = nullptr;
    }

    // Then destroy object manager
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
