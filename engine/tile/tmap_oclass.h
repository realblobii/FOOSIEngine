#ifndef TMAP_OCLASS_H
#define TMAP_OCLASS_H

#include <iostream>
#include <memory>
#include <string>

#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"

// Tilemap object class
class Tmap_OBJ : public Object {
public:
    
    

    Tmap_OBJ() {
        obj_class = "tmap"; // set the obj_class
    }

    void describe() const {
        std::cout << "OBJECT ID " << id
                  << " CLASS " << obj_class
                  << " COORDS (" << x << ", " << y << ", " << z << ")\n";
    }
};

// -----------------------------
// Register the object type
// This must be AFTER the full class definition
// Uses a static struct instead of lambda+bool to avoid compiler errors
// somehow this works but a lambda+bool doesnt lol
namespace {
    struct Tmap_OBJ_Registrar {
        Tmap_OBJ_Registrar() {
            ObjectFactory::registerClass("tmap", []() -> std::unique_ptr<Object> {
                return std::make_unique<Tmap_OBJ>();
            });
        }
    };

    // Static instance triggers registration at startup
    static Tmap_OBJ_Registrar global_Tmap_OBJ_registrar;
}

#endif // TMAP_OCLASS_H

