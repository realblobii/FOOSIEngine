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

    // Registers class with the OBJ Class system
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

#endif // OBJ_FACTORY_H
