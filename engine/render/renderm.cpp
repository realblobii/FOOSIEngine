// INCLUDES FOR TESTING PURPOSES
#include "testing/embedded_shaders.h"

// INCLUDES FOR ENGINE
#include "engine/render/renderm.h"
#include <algorithm>
#include <iostream>

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

renderPipeline::renderPipeline(Engine* eng)
    : engine(eng), registry(&eng->objMgr->registry)
{
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void renderPipeline::initTests(){
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
}
void renderPipeline::rainbowTriangle(){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
