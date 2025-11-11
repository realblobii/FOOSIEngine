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

    virtual void describe() const {
        std::cout << "Generic Object: " << obj_class << " - " << obj_subclass << "\n";
    }
};

#endif
