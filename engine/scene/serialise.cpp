#include "engine/scene/serialise.h"
#include "game/engine_api.h"
#include "game/main.h"
#include "engine/scene/scene_oclass.h"
#include <sstream>
#include <memory>
#include <stdexcept>

static inline std::string trim(const std::string &s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// extract scene name from header like: SCENE {name} {
static inline std::string extract_scene_name(const std::string &s) {
    const std::string keyword = "SCENE";
    auto pos = s.find(keyword);
    if (pos == std::string::npos)
        return "";

    // Everything after "SCENE"
    return trim(s.substr(pos + keyword.size()));
}


sceneData sceneManager::loadScene(
    const std::string& sceneFile,
    int baseX,
    int baseY,
    int baseZ
){
    sceneData sData;

    std::ifstream inFile(sFolder + "/" + sceneFile);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open scene file: " + sceneFile);
    }

    std::string line;

    // ─────────────────────────────
    // Parse header
    // ─────────────────────────────
    while (std::getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;

        sData.scene_name = extract_scene_name(line);
        break;
    }

    if (sData.scene_name.empty())
        sData.scene_name = sceneFile;

    // ─────────────────────────────
    // Create scene root object
    // ─────────────────────────────
    Object* baseObj = Instantiate("scene", "", baseX, baseY, baseZ);
    if (!baseObj)
        throw std::runtime_error("Failed to instantiate scene object");

    Scene_OBJ* scnObj = dynamic_cast<Scene_OBJ*>(baseObj);
    if (!scnObj)
        throw std::runtime_error("Scene object is not Scene_OBJ");

    scnObj->scnName = sData.scene_name;

    // Attach scene object to root
    engine->objMgr->addChild(engine->objMgr->getRoot(), scnObj);

    // ─────────────────────────────
    // Parse scene contents
    // ─────────────────────────────
    while (std::getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.find('}') != std::string::npos) break;

        if (line.back() == ';')
            line.pop_back();

        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) continue;

        // ───────── OBJECT ─────────
        if (cmd == "OBJECT") {
            std::string clsdot;
            int x = 0, y = 0, z = 0;

            if (!(iss >> clsdot >> x >> y >> z))
                continue;

            auto dot = clsdot.find('.');
            std::string obj_class    = (dot == std::string::npos) ? clsdot : clsdot.substr(0, dot);
            std::string obj_subclass = (dot == std::string::npos) ? ""      : clsdot.substr(dot + 1);

            Object* obj = Instantiate(
                obj_class,
                obj_subclass,
                x + baseX,
                y + baseY,
                z + baseZ
            );

            if (!obj) continue;

            engine->objMgr->addChild(scnObj, obj);
            sData.scene_obj_ids.push_back(obj->id);
        }

        // ───────── TILEMAP ─────────
        else if (cmd == "TILEMAP") {
            std::string path;
            int x = 0, y = 0, z = 0;

            if (!(iss >> path >> x >> y >> z))
                continue;

            auto ids = loadTilemapIds(
                sFolder + "/" + path,
                x + baseX,
                y + baseY,
                z + baseZ
            );

            for (int id : ids) {
                Object* tileObj = nullptr;

                for (auto& objPtr : engine->objMgr->registry) {
                    if (objPtr && objPtr->id == id) {
                        tileObj = objPtr.get();
                        break;
                    }
                }

                if (tileObj) {
                    engine->objMgr->addChild(scnObj, tileObj);
                }
            }

            sData.scene_obj_ids.insert(
                sData.scene_obj_ids.end(),
                ids.begin(),
                ids.end()
            );
        }

        // ───────── NESTED SCENE ─────────
        else if (cmd == "SCENE") {
            std::string path;
            int x = 0, y = 0, z = 0;

            if (!(iss >> path >> x >> y >> z))
                continue;

            sceneData nested = loadScene(
                path,
                baseX + x,
                baseY + y,
                baseZ + z
            );

            // Find the nested scene object by ID
            Scene_OBJ* nestedObj = nullptr;
            for (auto& objPtr : engine->objMgr->registry) {
                auto* s = dynamic_cast<Scene_OBJ*>(objPtr.get());
                if (s && s->scnName == nested.scene_name) {
                    nestedObj = s;
                    break;
                }
            }

            if (nestedObj) {
                engine->objMgr->addChild(scnObj, nestedObj);
            }

            sData.scene_obj_ids.insert(
                sData.scene_obj_ids.end(),
                nested.scene_obj_ids.begin(),
                nested.scene_obj_ids.end()
            );
        }
    }

    inFile.close();

    loadedScenes[sceneFile] = sData.scene_obj_ids;

    engine->objMgr->printTree(engine->objMgr->getRoot());

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