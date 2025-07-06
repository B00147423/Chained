// Engine.cpp
#include "../headers/Engine.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <windows.h>
#include "../headers/GameState.h"
#include "../headers/RenderService.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <vector>
#include <string>

namespace Chained { 
    Engine::Engine() {}

    Engine::~Engine() {
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        glfwTerminate();
    }

    bool Engine::init() {
        bool success = initGLFW() && initOpenGL();
        if (success) {
            RenderService::init(SCREEN_WIDTH, SCREEN_HEIGHT);
            
            // Setup ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsLight();

            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330");
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
        currentState = std::move(initialState);
        currentState->onEnter();

        double lastTime = glfwGetTime();

        // --- OPTIONAL: Window size selection (only in editor mode) ---
#ifdef CH_EDITOR
        bool windowSizeSelected = false;
        int selectedWidth = SCREEN_WIDTH;
        int selectedHeight = SCREEN_HEIGHT;
        bool fullscreen = false;
        std::vector<std::pair<std::string, std::pair<int, int>>> resolutions = {
            {"1280x720", {1280, 720}},
            {"1600x900", {1600, 900}},
            {"1920x1080", {1920, 1080}},
            {"2560x1440", {2560, 1440}},
        };

        while (!windowSizeSelected) {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Select Window Size", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::Text("Choose a resolution:");
            for (auto& res : resolutions) {
                if (ImGui::Button(res.first.c_str())) {
                    selectedWidth = res.second.first;
                    selectedHeight = res.second.second;
                    windowSizeSelected = true;
                    fullscreen = false;
                }
            }
            if (ImGui::Button("Fullscreen")) {
                windowSizeSelected = true;
                fullscreen = true;
            }
            ImGui::End();
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        // --- Recreate window with selected size ---
        if (fullscreen) {
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, nullptr, 100, 100, selectedWidth, selectedHeight, 0);
        }
#endif

        // --- Main loop ---
        while (!glfwWindowShouldClose(window)) {
            double now = glfwGetTime();
            float deltaTime = static_cast<float>(now - lastTime);
            lastTime = now;

            glfwPollEvents();

            // Start the ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            currentState->update(deltaTime);
            currentState->render();

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        currentState->onExit();
    }
}

