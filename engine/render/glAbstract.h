#include "engine/enginem.h"


class vao{
    public:
        vao();
        ~vao();
        void bind();
    private:
        unsigned int VAO;
};
class vbo {
public:
    vbo(const float* vertices, size_t count);
    ~vbo();
private:
    unsigned int VBO;
};
