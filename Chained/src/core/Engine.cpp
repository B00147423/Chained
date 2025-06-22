// Engine.cpp
#include "../headers/Engine.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Chained {
    Engine::Engine() {}

    Engine::~Engine() {
        glfwTerminate();
    }

    bool Engine::init() {
        return initGLFW() && initOpenGL();
    }

    bool Engine::initGLFW() {
        if (!glfwInit()) return false;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

        window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chained", nullptr, nullptr);
        if (!window) return false;

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);  // VSync

        return true;
    }

    bool Engine::initOpenGL() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
    

    #ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback([](GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* message, const void*) {
            std::cerr << "[GL DEBUG] " << message << "\n";
            }, nullptr);
    #endif

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
            glViewport(0, 0, w, h);
            });

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        return true;
    }

    void Engine::run(std::unique_ptr<GameState> initialState) {
        using namespace Chained;

        currentState = std::move(initialState);
        currentState->onEnter();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            currentState->update(0);
            currentState->render();

            glfwSwapBuffers(window);
        }

        currentState->onExit();
    }
}
