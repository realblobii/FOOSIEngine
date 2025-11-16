#include "engine/obj/obj_mgr.h"
#include <fstream>
#include <iostream>
#include "game/objclass.h"

int counter = 0;

//----------------------------------
// Object texture management
//----------------------------------
void Object::resolveTexture(const objManager& mgr) {
    // find this object's texture from JSON based on texref
    for (const auto& data : mgr.objectDefs) {
        if (data.obj_class == obj_class && data.obj_subclass == obj_subclass) {
            // JSON uses data.texture as the "default" texref,
            // but you could extend this to handle more refs if needed.
            if (texref == "default") {
                texture = data.texture;
            }
            
            return;
        }
    }

    std::cerr << "resolveTexture: failed to find texture for "
              << obj_class << ":" << obj_subclass
              << " texref=" << texref << "\n";
}

void Object::setTex(const std::string& newRef, const objManager& mgr) {
    texref = newRef;
    resolveTexture(mgr);
}

//----------------------------------
// objManager
//----------------------------------

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
    auto obj = ObjectFactory::create(obj_class);
    if (!obj) return nullptr;

    obj->obj_class = obj_class;
    obj->obj_subclass = obj_subclass;

    return obj;
}

void objManager::printRegistry() const {
    std::cout << "=== Object Registry ===\n";
    for (const auto& obj : registry)
        if (obj) obj->describe();
    std::cout << "=======================\n";
}

Object* objManager::instantiate(const std::string& obj_class,
                                const std::string& obj_subclass,
                                float x, float y, float z)
{
    auto obj = obj_load(obj_class, obj_subclass);
    if (!obj) {
        std::cerr << "Failed to instantiate: " << obj_class
                  << ":" << obj_subclass << "\n";
        return nullptr;
    }

    obj->x = x;
    obj->y = y;
    obj->z = z;
    obj->id = counter++;
    obj->texref = "default";

    // Resolve texture using current texref
    obj->resolveTexture(*this);

    registry.push_back(std::move(obj));
    return registry.back().get();
}
