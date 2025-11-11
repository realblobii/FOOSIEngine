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
    objManager(const std::string& objFile);

    // Creates an object given obj_class and obj_subclass
    std::unique_ptr<Object> obj_load(const std::string& obj_class, const std::string& obj_subclass);

private:
    // Stores textures per obj_class + obj_subclass
    struct ObjectData {
        std::string obj_class;
        std::string obj_subclass;
        std::string texture;
    };

    std::vector<ObjectData> objectDefs;
};

#endif
