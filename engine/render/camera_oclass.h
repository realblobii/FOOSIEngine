#ifndef CAMERA_OCLASS_H
#define CAMERA_OCLASS_H

#include <iostream>
#include <memory>
#include <string>

#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"

// Camera object class
class Camera_OBJ : public Object {
public:
    
    void describe() const {
            if (id != 0){
            std::cout << "OBJECT ID " << id
                    << "CLASS camera"
                    << " NAME " << objName
                    << " GLOBAL COORDS (" << x << ", " << y << ", " << z << ")"
                    << " LOCAL COORDS (" << lx << ", " << ly << ", " << lz << ")\n";
        }}

    Camera_OBJ() {
        obj_class = "camera"; // set the obj_class
        invis = true; // camera should not be rendered as a visible object
    }
};

// -----------------------------
// Register the object type
// This must be AFTER the full class definition
// Uses a static struct instead of lambda+bool to avoid compiler errors
// somehow this works but a lambda+bool doesnt lol
namespace {
    struct Camera_OBJ_Registrar {
        Camera_OBJ_Registrar() {
            ObjectFactory::registerClass("camera", []() -> std::unique_ptr<Object> {
                return std::make_unique<Camera_OBJ>();
            });
        }
    };

    // Static instance triggers registration at startup
    static Camera_OBJ_Registrar global_Camera_OBJ_registrar;
}

#endif // CAMERA_OCLASS_H

