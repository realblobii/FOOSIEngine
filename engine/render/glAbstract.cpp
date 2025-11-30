#include "engine/render/glAbstract.h"

GLsizei stride = 8 * sizeof(float); // 3 pos + 3 color + 2 uv = 8 floats

vao::vao(){
    glGenVertexArrays(1, &VAO);
    // Do NOT bind here permanently: binding is harmless here but we'll re-bind
    // where needed. Keep constructor simple.
    glBindVertexArray(VAO);
}
vao::~vao(){
    glDeleteVertexArrays(1, &VAO);
}
void vao::bind(){
    glBindVertexArray(VAO);
}

vbo::vbo(const float* vertices, size_t count) {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); // normal/color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    this->floatCount = static_cast<GLsizei>(count);
}

void vbo::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); // normal/color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
}


void vbo::update(const float* data, size_t count) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // <- was id
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_DYNAMIC_DRAW);
    this->floatCount = static_cast<GLsizei>(count);
}



vbo::~vbo(){
    glDeleteBuffers(1, &VBO);
}


