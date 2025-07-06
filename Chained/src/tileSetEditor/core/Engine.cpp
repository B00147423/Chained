// Engine.cpp
#include "../headers/Engine.h"
#include <memory>
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <windows.h>
#include "../headers/GameState.h"
#include "../headers/RenderService.h"
#include "../Game/uiStates/MainMenu.h"
#include "../Game/uiStates/DebugEditorState.h"
#include "../headers/AppState.h"
namespace Chained {
    Engine::Engine() {}

    Engine::~Engine() {
        glfwTerminate();
    }

    bool Engine::init() {
        bool success = initGLFW() && initOpenGL();
        if (success) {
            RenderService::init(SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        return success;
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
    void Engine::exitRunLoop() { keepRunning = false; }
    void Engine::run(std::unique_ptr<GameState> state) {
        currentState = std::move(state);
        currentState->onEnter();

        keepRunning = true;
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window) && keepRunning) {
            double now = glfwGetTime();
            float deltaTime = static_cast<float>(now - lastTime);
            lastTime = now;

            glfwPollEvents();
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            currentState->update(deltaTime);
            currentState->render();

            glfwSwapBuffers(window);
        }
        currentState->onExit();
    }

    AppState Engine::runMainMenu() {
        auto menu = std::make_unique<MainMenu>(this);
        AppState result = menu->result;
        run(std::move(menu));
        return result;
    }
    AppState Engine::runEditor() {
        auto editor = std::make_unique<DebugEditorState>(this);
        AppState result = editor->result;
        run(std::move(editor));
        return result;
    }
}
