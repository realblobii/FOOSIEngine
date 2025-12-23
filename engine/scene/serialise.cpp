#include "engine/scene/serialise.h"
#include "game/engine_api.h"
#include "game/main.h"
#include "engine/scene/scene_oclass.h"
#include <sstream>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <filesystem>

static inline std::string trim(const std::string &s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// extract scene name from header like: SCENE {name} {
static inline std::string extract_scene_name(const std::string &s) {
    const std::string keyword = "#SCNDEF";
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
    Object* baseObj = Instantiate("scene", "", "ROOT", baseX, baseY, baseZ);
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
    // Stack to track parent objects for nested blocks
    std::vector<Object*> parentStack;
    Object* mostRecentObj = nullptr;

    while (std::getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.find("#ENDSCN") != std::string::npos) break;

        // Handle block open/close (accept closing brace with optional semicolon: "}" or "};")
        if (line == "{") {
            if (mostRecentObj) {
                parentStack.push_back(mostRecentObj);
            } else {
                // No object before, so push root scene object
                parentStack.push_back(scnObj);
            }
            continue;
        }
        if (line == "}" || line == "};") {
            if (!parentStack.empty()) parentStack.pop_back();
            // If the closing brace had a semicolon, that terminates the mostRecentObj
            if (line == "};") {
                mostRecentObj = nullptr;
            }
            continue;
        }

        bool endsWithSemicolon = false;
        if (line.back() == ';') {
            line.pop_back();
            endsWithSemicolon = true;
        }

        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) continue;

        // ───────── OBJECT ─────────
        if (cmd == "OBJECT") {
            std::string clsdot;
            int x = 0, y = 0, z = 0;
            std::string name;

            if (!(iss >> name >> clsdot >> x >> y >> z))
                continue;
            
            auto dot = clsdot.find('.');
            std::string obj_class    = (dot == std::string::npos) ? clsdot : clsdot.substr(0, dot);
            std::string obj_subclass = (dot == std::string::npos) ? ""      : clsdot.substr(dot + 1);

            Object* obj = Instantiate(
                obj_class,
                obj_subclass,
                name,
                x + baseX,
                y + baseY,
                z + baseZ
            );

            if (!obj) continue;

            // Add as child to the current parent (top of stack or root scene)
            Object* parent = parentStack.empty() ? scnObj : parentStack.back();
            engine->objMgr->addChild(parent, obj);
            sData.scene_obj_ids.push_back(obj->id);

            mostRecentObj = obj;
            if (endsWithSemicolon) {
                mostRecentObj = nullptr;
            }
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

            // Add as child to the current parent (top of stack or root scene)
            Object* parent = parentStack.empty() ? scnObj : parentStack.back();
            if (nestedObj) {
                engine->objMgr->addChild(parent, nestedObj);
            }

            sData.scene_obj_ids.insert(
                sData.scene_obj_ids.end(),
                nested.scene_obj_ids.begin(),
                nested.scene_obj_ids.end()
            );

            mostRecentObj = nestedObj;
            if (endsWithSemicolon) {
                mostRecentObj = nullptr;
            }
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

// Helper to check if a Scene_OBJ refers to a nested scene and to get its file name
static inline std::string scene_filename_from_name(const std::string &name) {
    if (name.size() >= 6 && name.substr(name.size()-6) == ".fscn") return name;
    return name + ".fscn";
}

// Strip trailing .fscn extension if present
static inline std::string strip_fscn_ext(const std::string &name) {
    if (name.size() >= 6 && name.substr(name.size()-6) == ".fscn") return name.substr(0, name.size()-6);
    return name;
}

// Recursive writer for objects. Scenes (Scene_OBJ) are written as SCENE references and not expanded.
static void write_object_recursive(std::ostream &out, Object *obj, Object *sceneRoot, int indent=0) {
    auto indent_str = std::string(indent, ' ');

    // If this object itself is a scene (nested scene), write a SCENE reference and do not expand
    if (auto *s = dynamic_cast<Scene_OBJ*>(obj)) {
        int rx = static_cast<int>(std::lround(obj->x - sceneRoot->x));
        int ry = static_cast<int>(std::lround(obj->y - sceneRoot->y));
        int rz = static_cast<int>(std::lround(obj->z - sceneRoot->z));
        out << indent_str << "SCENE " << scene_filename_from_name(s->scnName)
            << " " << rx << " " << ry << " " << rz << ";\n";
        return;
    }

    // Regular object
    std::string fullcls = obj->obj_class;
    if (!obj->obj_subclass.empty()) fullcls += "." + obj->obj_subclass;

    int rx = static_cast<int>(std::lround(obj->x - sceneRoot->x));
    int ry = static_cast<int>(std::lround(obj->y - sceneRoot->y));
    int rz = static_cast<int>(std::lround(obj->z - sceneRoot->z));

    auto &children = obj->getChildren();
    if (children.empty()) {
        out << indent_str << "OBJECT " << fullcls << " " << rx << " " << ry << " " << rz << ";\n";
    } else {
        out << indent_str << "OBJECT " << fullcls << " " << rx << " " << ry << " " << rz << "\n";
        out << indent_str << "{\n";
        for (auto *c : children) {
            write_object_recursive(out, c, sceneRoot, indent+4);
        }
        out << indent_str << "};\n";
    }
}

sceneData sceneManager::saveScene(const std::string& sceneName){
    sceneData sData;

    // Determine output filename (ensure .fscn extension)
    std::string outFile = sceneName;
    if (outFile.size() < 6 || outFile.substr(outFile.size()-6) != ".fscn") {
        outFile = outFile + ".fscn";
    }

    // Find the scene object in the registry
    Scene_OBJ* sceneRoot = nullptr;
    for (auto &p : engine->objMgr->registry) {
        auto *s = dynamic_cast<Scene_OBJ*>(p.get());
        if (!s) continue;
        if (s->scnName == sceneName || scene_filename_from_name(s->scnName) == outFile || s->scnName + ".fscn" == outFile) {
            sceneRoot = s;
            break;
        }
    }

    if (!sceneRoot) {
        throw std::runtime_error("Scene not found: " + sceneName);
    }

    // Ensure the scene folder exists and open output file inside it
    try {
        std::filesystem::path dir(sFolder);
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Could not create scene directory: ") + e.what());
    }

    std::ofstream out(sFolder + "/" + outFile);
    if (!out.is_open()) {
        throw std::runtime_error("Could not open scene file for writing: " + outFile);
    }

    // Header: use scene name without extension for the #SCNDEF line
    std::string header_name = strip_fscn_ext(sceneRoot->scnName);
    sData.scene_name = header_name;
    out << "#SCNDEF " << header_name << "\n";
    out << "{\n";

    // Iterate immediate children and write them. Nested Scene_OBJ children are written as SCENE refs and not expanded.
    for (auto *c : sceneRoot->getChildren()) {
        write_object_recursive(out, c, sceneRoot, 4);
    }

    out << "};\n";
    out << "#ENDSCN\n";

    out.close();

    return sData;
}