#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "./headers/resourceManager.h"
#include "./headers/spriteRenderer.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cassert>

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;

bool initGlad();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void processInput(GLFWwindow* window, glm::vec2& position, float speed, float deltaTime);

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

    // Load shaders and textures here
    using namespace Chained;
    ResourceManager::get()->addSearchPath("assets/shaders");
    ResourceManager::get()->addSearchPath("assets/textures");

    auto shader = ResourceManager::get()->loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
    auto texture = ResourceManager::get()->loadTexture("awesomeface.png", true, "player");

    auto renderer = std::make_shared<SpriteRenderer>(shader);
    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT), 0.0f,
        -1.0f, 1.0f
    );


    shader->use();
    shader->setUniform("image", 0);
    shader->setUniform("projection", projection);  // this is needed once
    // Basic render loop
    glm::vec2 playerPos(200.0f, 200.0f);    // Initial player position
    float playerSpeed = 200.0f;
    float lastFrame = static_cast<float>(glfwGetTime());
    float currentFrame = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(window, playerPos, playerSpeed, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer->DrawSprite(texture, playerPos, glm::vec2(50, 50), 0.0f);

        glfwSwapBuffers(window);
    }


    ResourceManager::get()->clear();
    glfwTerminate();
    return 0;
}

bool initGlad() {
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void processInput(GLFWwindow* window, glm::vec2& position, float speed, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position.y -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position.y += speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position.x -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position.x += speed * deltaTime;
}


void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cerr << "[GL DEBUG] (" << id << "): " << message << "\n";
}
