#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>


class vao {
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
    void bind();
    GLsizei getCount() const { return floatCount / 8; }
    void update(const float* data, size_t count);
private:
    unsigned int VBO;
    GLsizei floatCount;
};

class fbo {
public:
    fbo(int width, int height);
    ~fbo();
    void bind();
    void unbind();
    GLuint getTexture() const { return texture; }
private:
    GLuint FBO = 0;
    GLuint texture = 0;
    GLuint RBO = 0;
    int w, h;
};
