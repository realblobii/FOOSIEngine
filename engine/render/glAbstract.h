#include "engine/enginem.h"

unsigned int VBO,VAO;
GLsizei stride = 8 * sizeof(float);
class vao{
    public:
        vao();
        void bind();
};
class vbo{
    public:
        vbo(float vertices[]);
};