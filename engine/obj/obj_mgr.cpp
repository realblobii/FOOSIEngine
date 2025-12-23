#include "engine/obj/obj_mgr.h"
#include <fstream>
#include <iostream>
#include "engine/coreclass.h"

int counter = 1;

Object* objManager::createRoot() {
    if (root) return root; // already created

    auto obj = std::make_unique<Object>();

    obj->id = 0;
    obj->obj_class = "";        // explicitly empty
    obj->obj_subclass = "";
    obj->texref = "";
    obj->texture = "";
    obj->invis = true;

    obj->x = 0.0f;
    obj->y = 0.0f;
    obj->z = 0.0f;

    // IMPORTANT: root should never resolve textures or accept properties
    // so we simply never call those systems on it.

    root = obj.get();
    registry.insert(registry.begin(), std::move(obj)); // force index 0

    return root;
}

//----------------------------------
// Object texture management
//----------------------------------
void Object::resolveTexture(const objManager& mgr) {
    // If manual mode is enabled, do not attempt to resolve from prototypes
    if (manualTex) return;

    // find this object's texture mapping from JSON based on texref name
    for (const auto& data : mgr.objectDefs) {
        if (data.obj_class == obj_class && data.obj_subclass == obj_subclass) {
            const Json::Value &textures = data.textures;
            if (textures.isObject()) {
                // prefer explicit texref name
                if (!texref.empty() && textures.isMember(texref)) {
                    texture = textures[texref].asString();
                    return;
                }
                // fall back to default
                if (textures.isMember("default")) {
                    texture = textures["default"].asString();
                    return;
                }
            } else if (textures.isString()) {
                // backward compatible: single default string
                texture = textures.asString();
                return;
            }
            break;
        }
    }

    std::cerr << "resolveTexture: failed to find texture for "
              << obj_class << ":" << obj_subclass
              << " texref=" << texref << "\n";
}

void Object::setTex(const std::string& newRef, const objManager& mgr) {
    if (manualTex) {
        // caller provided actual texture path when in manual mode
        texture = newRef;
        texref = newRef;
    } else {
        texref = newRef;
        resolveTexture(mgr);
    }
}

void objManager::printTree(Object* obj, const std::string& prefix, bool isLast) {
    if (!obj) return;

    std::cout << prefix;
    if (!prefix.empty()) std::cout << (isLast ? "└─ " : "├─ ");

    // Safe describe
    try {
        obj->describe();
    } catch (...) {
        std::cout << "[INVALID OBJECT]\n";
    }

    auto& children = obj->getChildren();
    std::string childPrefix = prefix + (isLast ? "   " : "│  ");

    for (size_t i = 0; i < children.size(); ++i) {
    if (children[i]) printTree(children[i], childPrefix, i == children.size() - 1);
    }

}



//----------------------------------
// objManager
//----------------------------------
void objManager::addChild(Object* parent, Object* child)
{
    if (!parent || !child) return;

    // Detach from old parent if needed
    if (Object* oldParent = child->getParent()) {
        removeChild(oldParent, child);
    }
    child->lx = child->x - parent->x;
    child->ly = child->y - parent->y;
    child->lz = child->z - parent->z;
    // Set new parent (non-owning)
    child->setParent(parent);

    // Store non-owning reference
    parent->getChildren().push_back(child);
}
void objManager::removeChild(Object* parent, Object* child)
{
    if (!parent || !child) return;

    auto& siblings = parent->getChildren();
    siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
    child->lx = 0.0f;
    child->ly = 0.0f;
    child->lz = 0.0f;
    // Clear parent reference
    child->setParent(nullptr);
}

objManager::objManager(const std::vector<std::string>& objFiles) {
    createRoot();

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;

    for (const auto &objFile : objFiles) {
        std::ifstream file(objFile);
        std::string tried = objFile;
        if (!file.is_open()) {
            // Try relative parent (useful when running from game/ directory)
            std::string alt = std::string("../") + objFile;
            std::ifstream f2(alt);
            if (f2.is_open()) {
                file.swap(f2);
                tried = alt;
            } else {
                std::cerr << "Warning: Failed to open JSON file: " << objFile << " or " << alt << "\n";
                continue;
            }
        }

        Json::Value root;
        if (!Json::parseFromStream(builder, file, &root, &errs)) {
            std::cerr << "Failed to parse JSON (" << tried << "): " << errs << "\n";
            continue;
        }

        const Json::Value objs = root["objects"];
        for (const auto& t : objs) {
            ObjectData data;
            data.obj_class = t["obj_class"].asString();
            data.obj_subclass = t["obj_subclass"].asString();
            // store full textures mapping so we can resolve by texref names
            data.textures = t["textures"];
            // optional properties block that can set object fields (like ScriptableObjects)
            if (t.isMember("properties")) {
                data.properties = t["properties"];
            }
            objectDefs.push_back(data);
        }
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
                                const std::string& name,
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
    obj->objName = name;

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
            // disallow prototype from toggling engine-defined visibility
            if (props.isMember("invis")) { props.removeMember("invis"); warned = true; }
            if (warned) {
                std::cerr << "objects.json: ignoring deprecated core fields (x,y,z,texref,invis) "
                          << "for prototype " << obj_class << ":" << obj_subclass << "\n";
            }
            obj->applyProperties(props);

            // Enforce engine semantics for core classes: scene and camera should always be invisible
            if (obj_class == "scene") obj->invis = true;
            if (obj_class == "camera") obj->invis = true;
            break;
        }
    }

    // Resolve texture using current texref (properties may have changed texref)
    obj->resolveTexture(*this);

    registry.push_back(std::move(obj));
    Object* objPtr = registry.back().get();
    objPtr->setParent(getRoot());
    getRoot()->getChildren().push_back(objPtr); // raw pointer, OK



    return registry.back().get();
}

void objManager::removeObjectsById(const std::vector<int>& ids) {
    if (ids.empty()) return;
    registry.erase(std::remove_if(registry.begin(), registry.end(), [&](const std::unique_ptr<Object>& o){
        if (!o) return false;
        return std::find(ids.begin(), ids.end(), o->id) != ids.end();
    }), registry.end());
}

