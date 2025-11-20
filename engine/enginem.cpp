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

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags|SDL_WINDOW_OPENGL);
        if (window) {
            std::cout << "Window Creation Success!" << std::endl;
        }
        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
        std::cerr << "OpenGL context failed! SDL_Error: "
                  << SDL_GetError() << std::endl;
                  isRunning = false;
        }
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            std::cerr << "Failed to initialize GLAD!" << std::endl;
        
        }
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

        sdl_sx = w;
        sdl_sy = h;

        
        if (!objMgr) {this->objMgr = new objManager("assets/objects.json");}
        if (!rPipeline){this->rPipeline = new renderPipeline(this);}
        rPipeline->initTests();


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
    //SDL_RenderClear(renderer);

    //rPipeline->renderAll();

    //SDL_RenderPresent(renderer);

    Uint32 frameTime = SDL_GetTicks() - currentTime;
    if (frameTime < TARGET_FRAME_TIME)
    {
      SDL_Delay(static_cast<Uint32>(TARGET_FRAME_TIME - frameTime));
    }
    rPipeline->rainbowTriangle();
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
    glDeleteVertexArrays(1,  &rPipeline->VAO);
    glDeleteBuffers(1, &rPipeline->VBO);
    glDeleteProgram(rPipeline->shaderProgram);
    SDL_GL_DeleteContext(glContext);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Engine Closed Successfully\n";
}
