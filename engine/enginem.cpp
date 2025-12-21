#include "engine/enginem.h"
#include "engine/render/renderm.h"  
 


Engine::Engine() {}
Engine::~Engine() {}

void Engine::Init(const char* title, int w, int h, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        int flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN;

        std::cout << "SDL Successfully Initialised!" << std::endl;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
                 SDL_WINDOW_OPENGL | flags | SDL_WINDOW_ALLOW_HIGHDPI);

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

        // query actual GL drawable size (handles high-DPI and fullscreen-desktop)
        int actual_w = w, actual_h = h;
        SDL_GL_GetDrawableSize(window, &actual_w, &actual_h);
        sdl_sx = actual_w;
        sdl_sy = actual_h;
        // store requested logical resolution so renderer can scale/stretch to framebuffer
        this->virt_sx = w;
        this->virt_sy = h;
        glViewport(0, 0, sdl_sx, sdl_sy);

        
        if (!objMgr) {this->objMgr = new objManager("demo/objects.json");}
        if (!rPipeline){
            stbi_set_flip_vertically_on_load(true);
            this->rPipeline = new renderPipeline(this);}
        if (!mLnr){
            this->mLnr = new mListener();}
        if (!kLnr){
            this->kLnr = new kListener();}

        // create scene manager rooted at the game folder so scenes live under game/
        if (!sceneMgr) {
            this->sceneMgr = new sceneManager("demo/scn");
            
        }

    } else {
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        isRunning = false;
    }
    isRunning = true;
}


void Engine::handleEvents() {
    SDL_Event event;
while (SDL_PollEvent(&event)) {
    mLnr->listen(event);
    if (kLnr) kLnr->listen(event);
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

        if (!obj->getParent()) continue; // root or detached object
        
        obj->x = obj->getParent()->x + obj->lx;
        obj->y = obj->getParent()->y + obj->ly;
        obj->z = obj->getParent()->z + obj->lz;
    }
    // tick input listeners so "hold" handlers are invoked each frame
    if (mLnr) mLnr->tick();
    if (kLnr) kLnr->tick();

    Update();
}

void Engine::render() {
    
    rPipeline->renderAll();

}




void Engine::clean() {
 
    if (objMgr) {
        delete objMgr;
        objMgr = nullptr;
    }
    if(rPipeline){
        delete rPipeline;
        rPipeline = nullptr;
    }
    if (kLnr) {
        delete kLnr;
        kLnr = nullptr;
    }
    if (sceneMgr) {
        delete sceneMgr;
        sceneMgr = nullptr;
    }

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(glContext);
    SDL_Quit();
    std::cout << "Engine Closed Successfully\n";
}
