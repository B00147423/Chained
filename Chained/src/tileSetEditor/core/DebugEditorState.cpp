#include "../Game/uiStates/DebugEditorState.h"
#include <nlohmann/json.hpp>
// #include <imgui.h>  // TEMPORARILY DISABLED until ImGui is properly initialized
#include <fstream> // Add this include to resolve the incomplete type error for std::ofstream
#include "../headers/SpriteRenderer.h"
#include "../headers/Texture2D.h"
#include "../headers/Engine.h"
#include "../headers/resourceManager.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>  
using json = nlohmann::json;
namespace Chained {

    /* ───────────── constants ───────────── */
    constexpr int TILE = 32;
    constexpr char SAVE_PATH[] = "room.json";

    /* colour for each debug tile id */
    static glm::vec3 colorFor(int id)
    {
        switch (id) {
        case 0: return { 1.0f, 1.0f, 0.0f }; // bright yellow for floor
        case 1: return { 0.50f, 0.50f, 0.50f }; // wall
        case 2: return { 0.0f , 0.0f , 0.0f }; // corner
        case 3: return { 0.0f , 0.0f , 1.0f }; // door
        case 4: return { 0.0f , 1.0f , 0.0f }; // player
        case 5: return { 1.0f , 0.0f , 0.0f }; // enemy
        default:return { 1.0f , 1.0f , 1.0f }; // unknown
        }
    }
    void DebugEditorState::onExit() {}
    /* ───────── constructor ───────── */
    DebugEditorState::DebugEditorState(Engine* eng) : engine(eng) {}

    /* ───────── TileMap save/load ───────── */
    void DebugEditorState::TileMap::save(const char* path) const
    {
        json j{ {"w", w}, {"h", h}, {"tiles", tiles} };
        std::ofstream(path) << j.dump(2);
    }
    bool DebugEditorState::TileMap::load(const char* path)
    {
        std::ifstream f(path);
        if (!f.is_open()) return false;
        json j; f >> j;
        w = j["w"];  h = j["h"];
        tiles = j["tiles"].get<std::vector<int>>();
        if ((int)tiles.size() != w * h) tiles.assign(w * h, 0);
        return true;
    }

    /* ───────── onEnter ───────── */
    void DebugEditorState::onEnter()
    {
        std::cout << "ON ENTER EDITOR\n";
        auto& rm = *ResourceManager::get();
        auto shader = rm.loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
        renderer = std::make_unique<SpriteRenderer>(shader);

        // --- THIS BLOCK IS REQUIRED ---
        glm::mat4 projection = glm::ortho(
            0.0f, float(Engine::SCREEN_WIDTH),
            float(Engine::SCREEN_HEIGHT), 0.0f, -1.0f, 1.0f
        );
        shader->use();
        shader->setUniform("projection", projection);
        shader->setUniform("image", 0);
        // --- END BLOCK ---

        whiteTex = Texture2D::Create(1, 1, 0xFFFFFFFFu);
        map.load(SAVE_PATH);
    }
    /* ───────── update ───────── */
    void DebugEditorState::update(float /*dt*/)
    {
        /* hot-key: Esc returns to previous state */
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            // Just return out, or set a flag if you add menu switching
            return;
        }

        // Press S to save
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            map.save(SAVE_PATH);
            std::cout << "Saved!\n";
        }
        // Press L to load
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_L) == GLFW_PRESS) {
            map.load(SAVE_PATH);
            std::cout << "Loaded!\n";
        }

        /* ImGui palette - TEMPORARILY DISABLED until ImGui is properly initialized */
        /*
        ImGui::Begin("Palette");
        for (int id = 0; id <= 5; ++id) {
            ImGui::PushID(id);
            ImGui::ColorButton("##c", ImVec4(colorFor(id).r,
                colorFor(id).g,
                colorFor(id).b, 1.0f),
                0, ImVec2(24, 24));
            ImGui::SameLine();
            if (ImGui::RadioButton(std::to_string(id).c_str(), current == id))
                current = id;
            ImGui::PopID();
        }
        if (ImGui::Button("Save")) map.save(SAVE_PATH);
        ImGui::SameLine();
        if (ImGui::Button("Load")) map.load(SAVE_PATH);
        ImGui::End();

        auto& io = ImGui::GetIO();
        if (!io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            double mx, my; glfwGetCursorPos(glfwGetCurrentContext(), &mx, &my);
            int gx = int(mx) / TILE;
            int gy = int(my) / TILE;
            if (gx >= 0 && gx < map.w && gy >= 0 && gy < map.h)
                map.at(gx, gy) = current;
        }
        */

        // Simple mouse input without ImGui for now
        if (glfwGetMouseButton(engine->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double mx, my; 
            glfwGetCursorPos(engine->getWindow(), &mx, &my);
            int gx = int(mx) / TILE;
            int gy = int(my) / TILE;
            if (gx >= 0 && gx < map.w && gy >= 0 && gy < map.h)
                map.at(gx, gy) = current;
        }

        // Simple keyboard input for tile selection
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_1) == GLFW_PRESS) current = 1;
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_2) == GLFW_PRESS) current = 2;
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_3) == GLFW_PRESS) current = 3;
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_4) == GLFW_PRESS) current = 4;
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_5) == GLFW_PRESS) current = 5;
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_0) == GLFW_PRESS) current = 0;
    }

    /* ───────── render ───────── */
    void DebugEditorState::render()
    {
        std::cout << "IN EDITOR RENDER\n";
        // draw your map grid (like you have now)
        for (int y = 0; y < map.h; ++y)
            for (int x = 0; x < map.w; ++x)
            {
                glm::vec2 pos{ x * TILE, y * TILE };
                glm::vec3 col = colorFor(map.at(x, y));
                renderer->DrawSprite(
                    whiteTex,
                    pos,
                    { TILE, TILE },
                    0.0f,
                    col,
                    { 0,0,1,1 }
                );
            }
        // draw a big red box at 0,0 so you know render() is being called
        renderer->DrawSprite(
            whiteTex,
            glm::vec2(0, 0),
            glm::vec2(100, 100),
            0.0f,
            glm::vec3(1.0f, 0, 0),
            glm::vec4(0, 0, 1, 1)
        );
    }

} // namespace Chained
