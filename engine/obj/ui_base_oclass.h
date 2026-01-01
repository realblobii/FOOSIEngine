#ifndef UI_BASE_OCLASS_H
#define UI_BASE_OCLASS_H

#include <memory>
#include <string>

#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"

// Base UI object: renders a texture at screen coordinates (or NDC normalized coords).
// Subclasses can override behavior by registering via ObjectFactory::overrideCreate("ui","subclass", ...)
class UIBase_OBJ : public Object {
public:
    UIBase_OBJ() {
        obj_class = "ui";
        obj_subclass = ""; // general UI base
        // positioning: normalized coords take precedence over pixel coords
        registerFloatProperty("nx", nx);
        registerFloatProperty("ny", ny);
        registerFloatProperty("sx", sx);
        registerFloatProperty("sy", sy);
        // optional pixel size override (width/height)
        registerFloatProperty("w", w);
        registerFloatProperty("h", h);
        // allow prototype to select texref (and let resolveTexture fill `texture`)
        registerStringProperty("texref", texref);
    }

    float nx = -1.0f;
    float ny = -1.0f;
    float sx = 0.0f;
    float sy = 0.0f;
    float w = 0.0f; // 0 = use image native size
    float h = 0.0f;
};

namespace {
    struct UIBase_OBJ_Registrar {
        UIBase_OBJ_Registrar() {
            ObjectFactory::registerClass("ui", []() -> std::unique_ptr<Object> {
                return std::make_unique<UIBase_OBJ>();
            });
        }
    };
    static UIBase_OBJ_Registrar global_UIBase_OBJ_registrar;
}

#endif // UI_BASE_OCLASS_H
