#include "engine/enginem.h"
#include "engine/render/renderm.h"  


Engine::Engine() {}
Engine::~Engine() {}

void Engine::Init(const char* title, int w, int h, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN;

        std::cout << "SDL Successfully Initialised!" << std::endl;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

       window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

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
            isRunning = false;   // you need this!
            return;
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

        sdl_sx = w;
        sdl_sy = h;

        
        if (!objMgr) {this->objMgr = new objManager("assets/objects.json");}
        if (!rPipeline){
            stbi_set_flip_vertically_on_load(true);
            this->rPipeline = new renderPipeline(this);}


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
void Engine::update() {
    for (auto& obj : objMgr->registry){
        obj->Update();
    }
}

void Engine::render() {
    
    rPipeline->renderAll();

}



void Engine::loadTileMap(const std::string& jsonFile, int tileWidth, int tileHeight) {
    if (tileMap) {
        delete tileMap;
    }
    tileMap = new TileMap(this, jsonFile, tileWidth, tileHeight);
}





void Engine::clean() {
  /* for (auto& tex : textures){
    delete tex;
    textures.clear();
    } */

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

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(glContext);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Engine Closed Successfully\n";
}
