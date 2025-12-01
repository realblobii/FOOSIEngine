#pragma once

#include "engine/enginem.h"
#include "incl/stb_image.h"
#include <string>

class Texture {
public:
    Texture(const std::string filename);
    void bind();
private:
    unsigned int texture;
};
