#include "engine/render/glAbstract.h"

GLsizei stride = 32;

vao::vao(){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
}
vao::~vao(){
    glDeleteVertexArrays(1,&VAO);
}
void vao::bind(){
     glBindVertexArray(VAO);
}
vbo::vbo(const float* vertices, size_t count) {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void vbo::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}
vbo::~vbo(){
    glDeleteBuffers(1,&VBO);
}