#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "./headers/resourceManager.h"
#include "./headers/spriteRenderer.h"
#include "./headers/SpriteAtlas.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;

bool initGlad();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

int main(int argc, char** argv) {
    if (!glfwInit()) {
        std::cerr << "ERROR: Failed to initialize GLFW\n";
        return -1;
    }

    glfwSetErrorCallback([](int code, const char* msg) {
        std::cerr << "GLFW ERROR [" << code << "]: " << msg << "\n";
        });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chained", nullptr, nullptr);
    assert(window);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    if (!initGlad()) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

#ifdef _DEBUG
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
    }
#endif

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
        });
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    using namespace Chained;
    ResourceManager::get()->addSearchPath("assets/shaders");
    ResourceManager::get()->addSearchPath("assets/textures");

    auto shader = ResourceManager::get()->loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
    auto renderer = std::make_shared<SpriteRenderer>(shader);

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT), 0.0f,
        -1.0f, 1.0f
    );

    shader->use();
    shader->setUniform("image", 0);
    shader->setUniform("projection", projection);

    // Load Aseprite sprite sheet JSON + PNG
    SpriteAtlas atlas("assets/textures/sprites.json");

    // Load slices
    auto& heartSlice = atlas.getSlice("heart");
    auto& orcSwordSlice = atlas.getSlice("orc_sword");

    // Flip UVs for OpenGL (origin is bottom-left)
    glm::vec4 heartUV = heartSlice.uvRect;
    heartUV.y = 1.0f - heartUV.y - heartUV.w;

    glm::vec4 swordUV = orcSwordSlice.uvRect;
    swordUV.y = 1.0f - swordUV.y - swordUV.w;

    std::cout << "Heart UV: "
        << heartUV.x << ", " << heartUV.y << ", "
        << heartUV.z << ", " << heartUV.w << "\n";
    std::cout << "Sword UV: "
        << swordUV.x << ", " << swordUV.y << ", "
        << swordUV.z << ", " << swordUV.w << "\n";

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw "heart"
        renderer->DrawSprite(
            atlas.getTexture(),
            glm::vec2(100, 100),                          // Position
            glm::vec2(heartUV.z * 1000, heartUV.w * 1051),// Size in pixels
            0.0f,
            glm::vec3(1.0f),
            heartUV
        );

        // Draw "orc_sword"
        renderer->DrawSprite(
            atlas.getTexture(),
            glm::vec2(220, 100),                          // Position
            glm::vec2(swordUV.z * 1000, swordUV.w * 1051),// Size in pixels
            0.0f,
            glm::vec3(1.0f),
            swordUV
        );

        glfwSwapBuffers(window);
    }
}

bool initGlad() {
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;
    std::cerr << "[GL DEBUG] (" << id << "): " << message << "\n";
}
