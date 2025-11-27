// INCLUDES FOR ENGINE
#include "engine/render/renderm.h"
#include <algorithm>
#include <iostream>
#include "incl/stb_image.h"



renderPipeline::renderPipeline(Engine* eng)
    : engine(eng),
      registry(&eng->objMgr->registry),
      defaultShader("shader/default.vs", "shader/default.fs")   // construct here!
{
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    defaultSptr = &defaultShader;
    char infoLog[512];
}

void renderPipeline::initTests(){
    float vertices[] = {
    // positions        // colors         // tex coords
     0.5f,  0.5f, 0.0f,  1,0,0,  1.0f, 1.0f, // top right
     0.5f, -0.5f, 0.0f,  0,1,0,  1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,  0,0,1,  0.0f, 0.0f, // bottom left

    -0.5f, -0.5f, 0.0f,  0,0,1,  0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,  1,1,0,  0.0f, 1.0f, // top left
     0.5f,  0.5f, 0.0f,  1,0,0,  1.0f, 1.0f  // top right
};


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // STRIDE: 8 floats per vertex
GLsizei stride = 8 * sizeof(float);

// Position (location 0)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
glEnableVertexAttribArray(0);

// Color (location 1)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);

// TexCoord (location 2)
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
glEnableVertexAttribArray(2);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

        
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
   unsigned char* data = stbi_load("assets/grass.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

}
void renderPipeline::rainbowTriangle(){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        // draw our first triangle
        defaultSptr->use();
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(engine->getWindow());
}
void renderPipeline::renderAll() {
    if (!registry || registry->empty()) {
        std::cerr << "[renderPipeline] Warning: registry is empty or null\n";
        return;
    }

    // 1. Copy non-null object pointers
    std::vector<Object*> sortedObjects;
    sortedObjects.reserve(registry->size());
    for (const auto& obj : *registry)
        if (obj) sortedObjects.push_back(obj.get());

    // 2. Sort by Z → Y → X (isometric painter’s order)
    std::sort(sortedObjects.begin(), sortedObjects.end(),
        [](const Object* a, const Object* b) {
            if (a->z != b->z) return a->z < b->z;
            if (a->y != b->y) return a->y < b->y;
            return a->x < b->x;
        });

    // 3. Render all objects
    const int TILE_WIDTH  = 64;
    const int TILE_HEIGHT = 64;
    const int OFFSET_X = engine->sdl_sx/2; 
    const int OFFSET_Y = engine->sdl_sy/2;

    for (const auto* obj : sortedObjects) {
        if (!obj) continue;

        // Convert float XYZ → screen coordinates
        float screenXf = (obj->x - obj->y) * (TILE_WIDTH / 2.0f) + OFFSET_X;
        float screenYf = (obj->x + obj->y) * (10) - (obj->z * (42)) + OFFSET_Y;

        // Cast to int for SDL rendering
        int screenX = static_cast<int>(screenXf);
        int screenY = static_cast<int>(screenYf);

        // Load + render texture (cached)
        Texture* tex = engine->loadTexture(obj->texture, screenX, screenY, TILE_WIDTH, TILE_HEIGHT);
        if (!tex) {
            std::cerr << "[renderPipeline] Failed to load texture: "
                      << obj->texture << "\n";
            continue;
        }

        tex->render();
    }
}
