#ifndef OBJ_FACTORY_H
#define OBJ_FACTORY_H

#include <functional>
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>

#include "engine/obj/obj.h"

// ObjectFactory manages a registry of object creators
class ObjectFactory {
public:
    using Creator = std::function<std::unique_ptr<Object>()>;

    // Register a class with a name
    static void registerClass(const std::string& name, Creator creator) {
        getRegistry()[name] = std::move(creator);
    }

    // Create an object by name
    static std::unique_ptr<Object> create(const std::string& name) {
        auto it = getRegistry().find(name);
        if (it != getRegistry().end()) {
            return (it->second)();
        }
        std::cerr << "Unknown object class: " << name << "\n";
        return nullptr;
    }

private:
    static std::unordered_map<std::string, Creator>& getRegistry() {
        static std::unordered_map<std::string, Creator> registry;
        return registry;
    }
};

// -----------------------------
// Portable registration macro
// Use AFTER full class definition
// Works with string literals
#define REGISTER_OBJECT_TYPE(TYPE, NAME_STR)                   \
namespace {                                                     \
    struct TYPE##_Registrar {                                   \
        TYPE##_Registrar() {                                    \
            ObjectFactory::registerClass(NAME_STR, []() {      \
                return std::make_unique<TYPE>();               \
            });                                                \
        }                                                       \
    };                                                          \
    static TYPE##_Registrar global_##TYPE##_registrar;         \
}

#endif // OBJ_FACTORY_H
