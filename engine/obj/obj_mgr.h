#ifndef OBJ_LOAD_H
#define OBJ_LOAD_H

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <json/json.h>
#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"

class objManager {
public:
    // objFiles: list of JSON prototype files to load. Core classes are always required to be present.
    objManager(const std::vector<std::string>& objFiles);

    std::unique_ptr<Object> obj_load(const std::string& obj_class, const std::string& obj_subclass);
    Object* instantiate(const std::string& obj_class,
                                const std::string& obj_subclass,
                                float x, float y, float z);
    
    Object* createRoot();
    Object* getRoot() const { return root; }

    void printRegistry() const;

    // Remove objects by id (used by scene unloading)
    void removeObjectsById(const std::vector<int>& ids);

    // Registry of live objects
    std::vector<std::unique_ptr<Object>> registry;
    
    friend class Object;

    // obj_mgr.h
    void addChild(Object* parent, Object* child);
    void removeChild(Object* parent, Object* child);

    void printTree(Object* obj, const std::string& prefix = "", bool isLast = true);

        

private:
    struct ObjectData {
        std::string obj_class;
        std::string obj_subclass;
        std::string texture;
        Json::Value properties;
    };
    Object* root = nullptr;
    std::vector<ObjectData> objectDefs;
};

#endif
