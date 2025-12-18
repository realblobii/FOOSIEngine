#include "engine/scene/serialise.h"
#include "game/engine_api.h"
#include "game/main.h"
#include <sstream>
#include <stdexcept>

static inline std::string trim(const std::string &s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// extract scene name from header like: SCENE {name} {
static inline std::string extract_scene_name(const std::string &s) {
    auto a = s.find('{');
    auto b = s.find('}', a==std::string::npos?0:a);
    if (a==std::string::npos || b==std::string::npos || b<=a) return "";
    return trim(s.substr(a+1, b-a-1));
}

sceneData sceneManager::loadScene(const std::string& sceneFile, int baseX, int baseY, int baseZ){
    sceneData sData;
    std::ifstream inFile(sFolder + "/" + sceneFile);
    if(!inFile.is_open()){
        throw std::runtime_error("Could not open scene file: " + sceneFile);
    }

    std::string line;

    // header: SCENE {name} {
    while (std::getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;
        sData.scene_name = extract_scene_name(line);
        break;
    }
    if (sData.scene_name.empty()) sData.scene_name = sceneFile;

    // directives: space-separated tokens. semicolon at end is optional
    while (std::getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.find('}') != std::string::npos) break; // end of scene

        if (line.back() == ';') line.pop_back();

        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) continue;

        if (cmd == "OBJECT") {
            std::string clsdot; int x=0,y=0,z=0;
            if (!(iss >> clsdot)) continue;
            iss >> x >> y >> z;
            auto dot = clsdot.find('.');
            std::string obj_class = (dot==std::string::npos)?clsdot:clsdot.substr(0,dot);
            std::string obj_subclass = (dot==std::string::npos)?"":clsdot.substr(dot+1);
            Object* p = Instantiate(obj_class, obj_subclass, x + baseX, y + baseY, z + baseZ);
            if (p) sData.scene_obj_ids.push_back(p->id);

        } else if (cmd == "TILEMAP") {
            std::string path; int x=0,y=0,z=0;
            if (!(iss >> path)) continue;
            iss >> x >> y >> z;
            auto ids = loadTilemapIds(sFolder + "/" + path, x + baseX, y + baseY, z + baseZ);
            sData.scene_obj_ids.insert(sData.scene_obj_ids.end(), ids.begin(), ids.end());

        } else if (cmd == "SCENE") {
            std::string path; int x=0,y=0,z=0;
            if (!(iss >> path)) continue;
            iss >> x >> y >> z;
            sceneData nested = loadScene(path, baseX + x, baseY + y, baseZ + z);
            sData.scene_obj_ids.insert(sData.scene_obj_ids.end(), nested.scene_obj_ids.begin(), nested.scene_obj_ids.end());
        } else {
            // ignore unknown lines
        }
    }

    inFile.close();
    loadedScenes[sceneFile] = sData.scene_obj_ids;
    return sData;
}

sceneData sceneManager::unloadScene(const std::string& sceneFile){
    sceneData sData;
    auto it = loadedScenes.find(sceneFile);
    if (it == loadedScenes.end()) return sData;

    sData.scene_name = sceneFile;
    sData.scene_obj_ids = it->second;
    if (!sData.scene_obj_ids.empty() && engine && engine->objMgr) {
        engine->objMgr->removeObjectsById(sData.scene_obj_ids);
    }

    loadedScenes.erase(it);
    return sData;
}