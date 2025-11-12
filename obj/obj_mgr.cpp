#include "engine/obj/obj_mgr.h"
#include <fstream>
#include <iostream>
#include "game/objclass.h"

int counter = 0;

objManager::objManager(const std::string& objFile) {
    std::ifstream file(objFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << objFile << "\n";
        return;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;

    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        std::cerr << "Failed to parse JSON: " << errs << "\n";
        return;
    }

    const Json::Value objs = root["objects"];
    for (const auto& t : objs) {
        ObjectData data;
        data.obj_class = t["obj_class"].asString();
        data.obj_subclass = t["obj_subclass"].asString();
        data.texture = t["textures"]["default"].asString();
        objectDefs.push_back(data);
    }
    
}

std::unique_ptr<Object> objManager::obj_load(const std::string& obj_class, const std::string& obj_subclass) {
    // 1. Create object using the factory
    auto obj = ObjectFactory::create(obj_class);
    if (!obj) return nullptr;

    obj->obj_class = obj_class;
    obj->obj_subclass = obj_subclass;

    // 2. Look up texture from JSON definitions
    auto it = std::find_if(objectDefs.begin(), objectDefs.end(),
        [&](const ObjectData& data) {
            return data.obj_class == obj_class && data.obj_subclass == obj_subclass;
        });

    if (it != objectDefs.end()) {
        // Check if object has a 'texture' member
        
            obj->texture = it->texture;
        
        // For other object types, you could extend this with similar dynamic_cast checks
    } else {
        std::cerr << "Warning: subclass '" << obj_subclass << "' not found for class '" << obj_class << "'\n";
    }

    return obj;
}

void objManager::printRegistry() const {
    std::cout << "=== Object Registry ===\n";
    for (const auto& obj : registry) {
        if (obj) obj->describe();
    }
    std::cout << "=======================\n";
}


Object* objManager::instantiate(std::string obj_class, std::string obj_subclass, int x, int y, int z) {
    // Create the object (using your obj_load function)
    auto obj = obj_load(obj_class, obj_subclass);
    if (!obj) {
        std::cerr << "Failed to instantiate: " << obj_class << ":" << obj_subclass << "\n";
        return nullptr;
    }

    // Set position
    obj->x = x;
    obj->y = y;
    obj->z = z;
    obj->id = counter;
    counter++;

    // Store the object in the registry
    registry.push_back(std::move(obj));

    // Return a raw pointer (for convenience)
    return registry.back().get();
}
