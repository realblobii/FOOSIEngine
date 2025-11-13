#ifndef OBJ_H
#define OBJ_H

#include <string>
#include <memory>
#include <iostream>

class objManager; // forward declaration

class Object {
public:
    virtual ~Object() = default;

    std::string obj_class;
    std::string obj_subclass;
    std::string texref;   // texture reference, e.g. "default", "damaged"
    std::string texture;  // actual texture path

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    int id = 0;

    // Describe for debug
    virtual void describe() const {
        std::cout << "OBJECT ID " << id
                  << " CLASS " << obj_class << ":" << obj_subclass
                  << " TEXREF " << texref
                  << " TEXTURE " << texture
                  << " COORDS (" << x << ", " << y << ", " << z << ")\n";
    }

    // Called to update texture based on texref
    virtual void resolveTexture(const objManager& mgr);

    // Changes texref and updates texture accordingly
    virtual void setTex(const std::string& newRef, const objManager& mgr);
};

#endif
