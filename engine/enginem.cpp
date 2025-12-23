#include "engine/enginem.h"
#include "engine/render/renderm.h"
#ifdef USE_FOOGUI
#include "addons/foogui/foogui.h"
#endif  
 


Engine::Engine() {}
Engine::~Engine() {}

void Engine::Init(const char* cfgPath) {
    // Load configuration from JSON if present (defaults used if missing)
    try {
        std::ifstream cfg(cfgPath);
        if (cfg.is_open()) {
            Json::CharReaderBuilder b;
            Json::Value root;
            std::string errs;
            if (Json::parseFromStream(b, cfg, &root, &errs)) {
                if (root.isMember("window_title")) window_title = root["window_title"].asString();
                virt_sx = root.get("virt_sx", virt_sx).asInt();
                virt_sy = root.get("virt_sy", virt_sy).asInt();
                fullscreen = root.get("fullscreen", fullscreen).asBool();
                tile_width = root.get("tile_width", tile_width).asInt();
                tile_height = root.get("tile_height", tile_height).asInt();
                atlas_size = root.get("atlas_size", atlas_size).asInt();
                // scene folder where .fscn files live
                scene_folder = root.get("scene_folder", scene_folder).asString();
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Warning: failed to read config " << cfgPath << " : " << e.what() << std::endl;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        int flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN;

        std::cout << "SDL Successfully Initialised!" << std::endl;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, virt_sx, virt_sy,
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
        int actual_w = virt_sx, actual_h = virt_sy;
        SDL_GL_GetDrawableSize(window, &actual_w, &actual_h);
        sdl_sx = actual_w;
        sdl_sy = actual_h;
        // logical resolution is provided by config (virt_sx / virt_sy)
        glViewport(0, 0, sdl_sx, sdl_sy);

        
        if (!objMgr) {
            // Build the list of object prototype files: engine core classes plus game-provided lists in config
            std::vector<std::string> objFiles;
            objFiles.push_back("engine/coreclass.json");
            try {
                std::ifstream cfg(cfgPath);
                if (cfg.is_open()) {
                    Json::CharReaderBuilder b;
                    Json::Value root;
                    std::string errs;
                    if (Json::parseFromStream(b, cfg, &root, &errs)) {
                        if (root.isMember("object_files")) {
                            for (const auto &v : root["object_files"]) {
                                objFiles.push_back(v.asString());
                            }
                        }
                    }
                }
            } catch (...) {}

            this->objMgr = new objManager(objFiles);
        }
        if (!sceneMgr) {
            this->sceneMgr = new sceneManager(scene_folder);
        }
        if (!rPipeline){
            stbi_set_flip_vertically_on_load(true);
            this->rPipeline = new renderPipeline(this);
#ifdef USE_FOOGUI
            // integrate foogui addon layer if compiled in
            auto guil = std::make_unique<foogui::GuiLayer>(this, atlas_size);
            auto guilptr = guil.get();
            this->rPipeline->addLayer(std::move(guil));
            // example: set default font and a short demo string
            // Prefer local fonts managed by the addon; let discovery pick one if unspecified
            guilptr->setFont("", 24);
            guilptr->addText("Hello, UI Layer!");
#endif
        }
        if (!mLnr){
            this->mLnr = new mListener();}
        if (!kLnr){
            this->kLnr = new kListener();}

        // create scene manager rooted at the game folder so scenes live under configured scene folder
        

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
