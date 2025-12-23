#ifndef SCENE_OCLASS_H
#define SCENE_OCLASS_H

#include <iostream>
#include <memory>
#include <string>

#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"

// Tile object class
class Scene_OBJ : public Object {
public:
    
    void describe() const {
        std::cout << "OBJECT ID " << id
                  << " NAME " << scnName
                  << " GLOBAL COORDS (" << x << ", " << y << ", " << z << ")"
                  << " LOCAL COORDS (" << lx << ", " << ly << ", " << lz << ")\n";
    }
    bool invis = true;
    std::string scnName; 
    Scene_OBJ() {
        obj_class = "scene"; // set the obj_class
    }
};

// -----------------------------
// Register the object type
// This must be AFTER the full class definition
// Uses a static struct instead of lambda+bool to avoid compiler errors
// somehow this works but a lambda+bool doesnt lol
namespace {
    struct Scene_OBJ_Registrar {
        Scene_OBJ_Registrar() {
            ObjectFactory::registerClass("scene", []() -> std::unique_ptr<Object> {
                return std::make_unique<Scene_OBJ>();
            });
        }
    };

    // Static instance triggers registration at startup
    static Scene_OBJ_Registrar global_Scene_OBJ_registrar;
}

#endif // TILE_OCLASS_H

