#ifndef UI_TEXT_OCLASS_H
#define UI_TEXT_OCLASS_H

#include <iostream>
#include <memory>
#include <string>

#include "engine/obj/obj.h"
#include "engine/obj/obj_factory.h"
#include "engine/obj/ui_base_oclass.h"

// Simple UI text object class — instances are not rendered as textures;
// GuiLayer's text pipeline will render them instead.
class UIText_OBJ : public UIBase_OBJ {
public:
    UIText_OBJ() {
        obj_class = "ui";
        obj_subclass = "text";
        // properties available in prototype data and scene files
        registerStringProperty("text", text);
        registerStringProperty("font", font);
        registerIntProperty("size", size);
        // reuse nx,ny,sx,sy from base class
        // colors (not yet used by shader, but stored)
        registerFloatProperty("r", r);
        registerFloatProperty("g", g);
        registerFloatProperty("b", b);
        registerFloatProperty("a", a);
    }

    std::string text;
    std::string font; // font path or empty to use default discovery
    int size = 24;

    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
};

namespace {
    struct UIText_OBJ_Registrar {
        UIText_OBJ_Registrar() {
            // Register an explicit override creator for subclass 'text' of base class 'ui'
            ObjectFactory::overrideCreate("ui", "text", []() -> std::unique_ptr<Object> {
                return std::make_unique<UIText_OBJ>();
            });
        }
    };
    static UIText_OBJ_Registrar global_UITEXT_OBJ_registrar;
}

#endif // UI_TEXT_OCLASS_H
