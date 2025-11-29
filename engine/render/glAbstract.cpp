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


fbo::fbo(int width, int height) : w(width), h(height) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FBO:: Framebuffer is not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

fbo::~fbo() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &RBO);
}

void fbo::bind() { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }
void fbo::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
