    #ifndef OBJ_H
    #define OBJ_H

    #include <string>
    #include <memory>
    #include <iostream>
    #include <json/json.h>
    #include <unordered_map>
    #include <functional>

    class objManager; // forward declaration

    class Object {
    public:
        virtual ~Object() = default;

        std::string obj_class;
        std::string obj_subclass;
        std::string texref = "default";   
        std::string texture; 
        bool invis;

        std::vector<Object*>& getChildren()  { return children; }

    void setParent(Object* p) { parent = p; }
        Object* getParent() const {    return parent; }


        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float lx = 0.0f;
        float ly = 0.0f;
        float lz = 0.0f;
        int id = 0;

        // Describe for debug
        virtual void describe() const {
            if (id != 0){
            std::cout << "OBJECT ID " << id
                    << " CLASS " << obj_class << "." << obj_subclass
                    << " TEXREF " << texref
                    << " TEXTURE " << texture
                    << " GLOBAL COORDS (" << x << ", " << y << ", " << z << ")"
                    << " LOCAL COORDS (" << lx << ", " << ly << ", " << lz << ")\n";
        }
        else{
            std::cout << "ROOT ID 0\n";
        }}

        
        virtual void resolveTexture(const objManager& mgr);
        virtual void setTex(const std::string& newRef, const objManager& mgr);
        // Property system: derived classes can register named property setters
        // that are invoked when a prototype's properties block is applied.
        using PropertySetter = std::function<void(const Json::Value&)>;

        // Register a generic setter (usually called from derived class ctor)
        void registerProperty(const std::string& name, PropertySetter setter) {
            propertySetters[name] = std::move(setter);
        }

        // Convenience helpers to register common member types by reference
        void registerFloatProperty(const std::string& name, float &ref) {
            propertySetters[name] = [&ref](const Json::Value &v){ if (!v.isNull()) ref = v.asFloat(); };
        }
        void registerIntProperty(const std::string& name, int &ref) {
            propertySetters[name] = [&ref](const Json::Value &v){ if (!v.isNull()) ref = v.asInt(); };
        }
        void registerBoolProperty(const std::string& name, bool &ref) {
            propertySetters[name] = [&ref](const Json::Value &v){ if (!v.isNull()) ref = v.asBool(); };
        }
        void registerStringProperty(const std::string& name, std::string &ref) {
            propertySetters[name] = [&ref](const Json::Value &v){ if (!v.isNull()) ref = v.asString(); };
        }

        // Apply properties: iterate JSON keys and invoke any registered setters.
        // Unknown keys are ignored by default.
        virtual void applyProperties(const Json::Value& props) {
            if (!props || props.isNull()) return;
            for (const auto &name : props.getMemberNames()) {
                auto it = propertySetters.find(name);
                if (it != propertySetters.end()) {
                    it->second(props[name]);
                } else {
                    // unknown property: ignore silently (or log if desired)
                }
            }
        }
        virtual void Update(){}
    private:
        std::unordered_map<std::string, PropertySetter> propertySetters;

        std::vector<Object*> children; // raw pointers
        Object* parent = nullptr;
    };

    #endif
