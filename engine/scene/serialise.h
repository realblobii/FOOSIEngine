#ifndef SERIALISE_H
#define SERIALISE_H 

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "engine/obj/obj.h"

struct sceneData{
    std::string scene_name;
    // object ids returned from instantiation
    std::vector<int> scene_obj_ids;
};
class sceneManager{
    public:
        sceneManager(const std::string& fldr) : sFolder(fldr) {}
        // base offsets applied when instantiating (supports float offsets)
        sceneData loadScene(const std::string& sceneFile, float baseX = 0.0f, float baseY = 0.0f, float baseZ = 0.0f);
        sceneData unloadScene(const std::string& sceneFile);
        sceneData saveScene(const std::string& sceneName);

        Object* camera;
        bool isCamera = false;
    private:
        
        const std::string sFolder;
        std::unordered_map<std::string, std::vector<int>> loadedScenes;
};




#endif