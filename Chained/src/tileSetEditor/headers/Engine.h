#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameState.h"
#include "resourceManager.h"
#include "spriteRenderer.h"
#include "AppState.h"

namespace Chained {
    class Engine {
    public:

        enum class EngineState {
            Menu,
            Editor,
            Quit
        };
        static constexpr int SCREEN_WIDTH = 800;
        static constexpr int SCREEN_HEIGHT = 600;

        Engine();
        ~Engine();

        bool init();
        void run(std::unique_ptr<GameState> initialState);

        AppState runMainMenu();
        AppState runEditor();
        void exitRunLoop();

        GLFWwindow* getWindow() const { return window; }

    private:
        GLFWwindow* window = nullptr;
        std::shared_ptr<Chained::SpriteRenderer> renderer;
        std::unique_ptr<GameState> currentState;
        bool initGLFW();
        bool initOpenGL();
        bool keepRunning = true;
    };
}
