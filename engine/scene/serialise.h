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
        // base offsets applied when instantiating
        sceneData loadScene(const std::string& sceneFile, int baseX = 0, int baseY = 0, int baseZ = 0);
        sceneData unloadScene(const std::string& sceneFile);
        sceneData saveScene(const std::string& sceneName);

        Object* camera;
        bool isCamera = false;
    private:
        
        const std::string sFolder;
        std::unordered_map<std::string, std::vector<int>> loadedScenes;
};




#endif