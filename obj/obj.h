#ifndef OBJ_H
#define OBJ_H

#include <string>
#include <memory>
#include <iostream>

class Object {
public:
    virtual ~Object() = default;
    std::string obj_class;
    std::string obj_subclass;
    std::string texture;

    int x,y,z,id;

    virtual void describe() const {
        std::cout << "OBJECT ID " << id << " CLASS " << obj_class << ":" << obj_subclass << " TEXTURE " << texture << " COORDS " << x << " " << y << " " << z << std::endl;

    }
};

#endif
