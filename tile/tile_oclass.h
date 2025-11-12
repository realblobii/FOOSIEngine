#ifndef TILE_OCLASS_H
#define TILE_OCLASS_H

#include <iostream>
#include <memory>
#include <string>

#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"

// Tile object class
class Tile_OBJ : public Object {
public:
    

    Tile_OBJ() {
        obj_class = "tile"; // set the obj_class
    }
};

// -----------------------------
// Register the object type
// This must be AFTER the full class definition
// Uses a static struct instead of lambda+bool to avoid compiler errors
// somehow this works but a lambda+bool doesnt lol
namespace {
    struct Tile_OBJ_Registrar {
        Tile_OBJ_Registrar() {
            ObjectFactory::registerClass("tile", []() -> std::unique_ptr<Object> {
                return std::make_unique<Tile_OBJ>();
            });
        }
    };

    // Static instance triggers registration at startup
    static Tile_OBJ_Registrar global_Tile_OBJ_registrar;
}

#endif // TILE_OCLASS_H

