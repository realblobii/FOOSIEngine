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

    // Try to create an object by name without emitting an error
    static std::unique_ptr<Object> tryCreate(const std::string& name) {
        auto it = getRegistry().find(name);
        if (it != getRegistry().end()) {
            return (it->second)();
        }
        return nullptr;
    }

    // Override registration: allow a base class to define a creator for a specific subclass.
    using OverrideMap = std::unordered_map<std::string, std::unordered_map<std::string, Creator>>;

    // Register an override creator for a (baseClass, subclass) pair
    static void overrideCreate(const std::string& baseClass, const std::string& subclass, Creator creator) {
        getOverrides()[baseClass][subclass] = std::move(creator);
    }

    // Try to create using an override for baseClass+subclass without emitting error
    static std::unique_ptr<Object> tryCreateOverride(const std::string& baseClass, const std::string& subclass) {
        if (subclass.empty()) return nullptr;
        auto& ov = getOverrides();
        auto it = ov.find(baseClass);
        if (it != ov.end()) {
            auto it2 = it->second.find(subclass);
            if (it2 != it->second.end()) {
                return (it2->second)();
            }
        }
        return nullptr;
    }

    // Create an object by name; prints an error if not found
    static std::unique_ptr<Object> create(const std::string& name) {
        auto obj = tryCreate(name);
        if (obj) return obj;
        std::cerr << "Unknown object class: " << name << "\n";
        return nullptr;
    }

    // Accessor for the overrides map
    static OverrideMap& getOverrides() {
        static OverrideMap overrides;
        return overrides;
    }

private:
    static std::unordered_map<std::string, Creator>& getRegistry() {
        static std::unordered_map<std::string, Creator> registry;
        return registry;
    }
};

#endif // OBJ_FACTORY_H
