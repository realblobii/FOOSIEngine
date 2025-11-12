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

    std::unique_ptr<Object> obj_load(const std::string& obj_class, const std::string& obj_subclass);

    // Instantiates an object and stores it in the registry
    Object* instantiate(std::string obj_class, std::string obj_subclass, int x, int y, int z);

    // Registry of active game objects
    std::vector<std::unique_ptr<Object>> registry;

    void printRegistry() const;


private:
    struct ObjectData {
        std::string obj_class;
        std::string obj_subclass;
        std::string texture;
        int counter;
    };

    std::vector<ObjectData> objectDefs;
};

#endif
