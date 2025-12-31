#ifndef UI_TEXT_OCLASS_H
#define UI_TEXT_OCLASS_H

#include <iostream>
#include <memory>
#include <string>

#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"

// Simple UI text object class — instances are not rendered in world space;
// GuiLayer will pick them up and render into the UI pipeline instead.
class UIText_OBJ : public Object {
public:
    UIText_OBJ() {
        obj_class = "ui";
        obj_subclass = "text";
        // properties available in prototype data and scene files
        registerStringProperty("text", text);
        registerStringProperty("font", font);
        registerIntProperty("size", size);
        // normalized coords in [0..1]; if set (>=0) they take precedence
        registerFloatProperty("nx", nx);
        registerFloatProperty("ny", ny);
        // pixel coords (screen space)
        registerFloatProperty("sx", sx);
        registerFloatProperty("sy", sy);
        // colors (not yet used by shader, but stored)
        registerFloatProperty("r", r);
        registerFloatProperty("g", g);
        registerFloatProperty("b", b);
        registerFloatProperty("a", a);
    }

    std::string text;
    std::string font; // font path or empty to use default discovery
    int size = 24;

    float nx = -1.0f;
    float ny = -1.0f;

    float sx = 0.0f;
    float sy = 0.0f;

    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
};

namespace {
    struct UIText_OBJ_Registrar {
        UIText_OBJ_Registrar() {
            ObjectFactory::registerClass("ui", []() -> std::unique_ptr<Object> {
                return std::make_unique<UIText_OBJ>();
            });
        }
    };
    static UIText_OBJ_Registrar global_UITEXT_OBJ_registrar;
}

#endif // UI_TEXT_OCLASS_H
