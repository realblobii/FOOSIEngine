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
        // optional properties block that can set object fields (like ScriptableObjects)
        if (t.isMember("properties")) {
            data.properties = t["properties"];
        }
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

    // Apply subclass properties from objects.json (overrides defaults)
    // Enforcement: we DO NOT allow prototype JSON to set core instance
    // placement or engine-managed fields (x,y,z,texref). If these keys
    // are present in the properties block, emit a warning and ignore them.
    for (const auto &d : objectDefs) {
        if (d.obj_class == obj_class && d.obj_subclass == obj_subclass) {
            Json::Value props = d.properties; // make a copy we can modify
            bool warned = false;
            if (props.isMember("x")) { props.removeMember("x"); warned = true; }
            if (props.isMember("y")) { props.removeMember("y"); warned = true; }
            if (props.isMember("z")) { props.removeMember("z"); warned = true; }
            if (props.isMember("texref")) { props.removeMember("texref"); warned = true; }
            if (warned) {
                std::cerr << "objects.json: ignoring deprecated core fields (x,y,z,texref) "
                          << "for prototype " << obj_class << ":" << obj_subclass << "\n";
            }
            obj->applyProperties(props);
            break;
        }
    }

    // Resolve texture using current texref (properties may have changed texref)
    obj->resolveTexture(*this);

    registry.push_back(std::move(obj));
    return registry.back().get();
}

void objManager::removeObjectsById(const std::vector<int>& ids) {
    if (ids.empty()) return;
    registry.erase(std::remove_if(registry.begin(), registry.end(), [&](const std::unique_ptr<Object>& o){
        if (!o) return false;
        return std::find(ids.begin(), ids.end(), o->id) != ids.end();
    }), registry.end());
}

