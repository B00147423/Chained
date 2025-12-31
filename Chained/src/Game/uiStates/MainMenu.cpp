#include "MainMenu.h"
#include "TestState.h"
#include "../../headers/resourceManager.h"
#include "../../headers/Engine.h"
#ifdef CH_EDITOR
#include "../../headers/EditorState.h"
#endif
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


namespace Chained {

    // ───────────────────────────────────────────────
    MainMenu::MainMenu(Engine* eng) : engine(eng) {}

    // ───────────────────────────────────────────────
    void MainMenu::onEnter()
    {
        auto& rm = *ResourceManager::get();
        rm.addSearchPath("assets/shaders");
        rm.addSearchPath("assets/textures");

        auto shader = rm.loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
        renderer = std::make_shared<SpriteRenderer>(shader);

        glm::mat4 projection = glm::ortho(0.f, static_cast<float>(Engine::SCREEN_WIDTH),
            static_cast<float>(Engine::SCREEN_HEIGHT), 0.f, -1.f, 1.f);
        shader->use();
        shader->setUniform("projection", projection);
        shader->setUniform("image", 0);

        atlas = std::make_unique<SpriteAtlas>("assets/textures/sprites.json");

        float centerX = static_cast<float>(Engine::SCREEN_WIDTH) * 0.5f;
        float startY = static_cast<float>(Engine::SCREEN_HEIGHT) * 0.3f;
        float buttonSpacing = 120.0f;

        static constexpr glm::vec2 PlayBtnSize{ 183,  93 };
        static constexpr glm::vec2 SettingsBtnSize{ 179,  87 };
        static constexpr glm::vec2 ExitBtnSize{ 175,  81 };

        playBtn.pos = { centerX - PlayBtnSize.x * 0.5f, startY };
        settingsBtn.pos = { centerX - SettingsBtnSize.x * 0.5f, startY + buttonSpacing };
        exitBtn.pos = { centerX - ExitBtnSize.x * 0.5f, startY + buttonSpacing * 2 };

        // ---- STYLE: REMOVE ALL BORDERS & HIGHLIGHTS ----
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 10.0f;
        style.FrameRounding = 5.0f;
        style.GrabRounding = 5.0f;
        // Remove all background, border, and nav highlight colors
        style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0, 0, 0, 0);
        // (optional: also kill header and separator colors)
        style.Colors[ImGuiCol_Header] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_Separator] = ImVec4(0, 0, 0, 0);

        // Remove nav focus highlight entirely
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
    }


    void MainMenu::onExit() {}

    // ───────────────────────────────────────────────
    void MainMenu::update(float dt)
    {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        int winW, winH;
        glfwGetWindowSize(engine->getWindow(), &winW, &winH);

        double mx, my;
        glfwGetCursorPos(engine->getWindow(), &mx, &my);

        // convert to Engine's virtual space
        float px = static_cast<float>(mx) * (static_cast<float>(Engine::SCREEN_WIDTH) / winW);
        float py = static_cast<float>(my) * (static_cast<float>(Engine::SCREEN_HEIGHT) / winH);

        int mouse = glfwGetMouseButton(engine->getWindow(), GLFW_MOUSE_BUTTON_LEFT);
#ifdef CH_EDITOR
        if (glfwGetKey(engine->getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
            engine->run(std::make_unique<EditorState>(engine));
        }
#endif
        // handle each button
        handleInput(playBtn, px, py, mouse);
        handleInput(settingsBtn, px, py, mouse);
        handleInput(exitBtn, px, py, mouse);

        // if exit clicked → close window
        if (exitBtn.pressed && mouse == GLFW_RELEASE && exitBtn.hovered)
        {
            exitBtn.pressed = false;
            glfwSetWindowShouldClose(engine->getWindow(), GLFW_TRUE);
        }
        // basic scale pulse on *any* press
        clickScale += ((mouse == GLFW_PRESS ? 0.95f : 1.0f) - clickScale) * 10.f * dt;
        
        // Render ImGui UI
        renderImGuiUI();
    }

    // ───────────────────────────────────────────────
    void MainMenu::handleInput(MenuButton& btn,
        float px, float py,
        int mouseState)
    {
        // hover test
        btn.hovered =
            px >= btn.pos.x && px <= btn.pos.x + btn.size.x &&
            py >= btn.pos.y && py <= btn.pos.y + btn.size.y;

        // press logic
        if (btn.hovered && mouseState == GLFW_PRESS && !btn.pressed)
            btn.pressed = true;

        if (mouseState == GLFW_RELEASE && btn.pressed)
        {
            btn.pressed = false;
            if (btn.hovered)
            {
                if (btn.sliceName == std::string("play_btn"))
                    engine->run(std::make_unique<TestState>("scenes/testsix.json"));
                // TODO: open settings menu when you have it
            }
        }
    }

    // ───────────────────────────────────────────────
    void MainMenu::drawButton(const MenuButton& btn)
    {
        auto& slice = atlas->getSlice(btn.sliceName);
        glm::vec4 uv = slice.uvRect;
        uv.y = 1.f - uv.y - uv.w;               // flip Y - this is essential for correct texture rendering

        glm::vec3 tint = idleCol;
        if (btn.pressed)       tint = pressedCol;
        else if (btn.hovered)  tint = hoverCol;

        // outline
        if (btn.hovered && !btn.pressed)
        {
            glm::vec2 outSize = btn.size + glm::vec2(outlinePx * 2.f);
            glm::vec2 outPos = btn.pos - glm::vec2(outlinePx);
            renderer->DrawSprite(atlas->getTexture(), outPos, outSize,
                0.f, glm::vec3(0.f), uv);
        }

        renderer->DrawSprite(atlas->getTexture(),
            btn.pos,
            btn.size,
            0.f,
            tint,
            uv);
    }

    // ───────────────────────────────────────────────
    void MainMenu::renderImGuiUI()
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("MainMenu", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.3f);

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("CHAINED").x) * 0.5f);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "CHAINED");
        ImGui::PopFont();

        ImGui::Spacing();
        ImGui::Spacing();

        ImVec2 playBtnSize(183, 93);
        ImVec2 settingsBtnSize(179, 87);
        ImVec2 exitBtnSize(175, 81);

        ImTextureID atlasTexId = (ImTextureID)(intptr_t)atlas->getTexture()->m_id;

        // ----- PLAY BUTTON -----
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - playBtnSize.x) * 0.5f);
        auto& playSlice = atlas->getSlice("play_btn");
        ImVec2 playUv0(playSlice.uvRect.x, 1.0f - playSlice.uvRect.y);
        ImVec2 playUv1(playSlice.uvRect.x + playSlice.uvRect.z, 1.0f - (playSlice.uvRect.y + playSlice.uvRect.w));
        ImVec4 tint = ImGui::IsItemHovered() ? ImVec4(1,1,0.7f,1) : ImVec4(1,1,1,1);
        bool playPressed = ImGui::ImageButton("##playbtn", atlasTexId, playBtnSize, playUv0, playUv1, ImVec4(0,0,0,0), tint);

        // Overlay logic (bright test colors)
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        if (ImGui::IsItemActive())
            ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(255, 255, 0, 255), 10.0f, 0, 6.0f); // Yellow outline for pressed
        else if (ImGui::IsItemHovered())
            ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(0, 200, 255, 255), 10.0f, 0, 4.0f); // Blue outline for hover

        if (playPressed) {
            engine->run(std::make_unique<TestState>("scenes/mainMenu"));
        }
        ImGui::Spacing();

        // ----- SETTINGS BUTTON -----
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - settingsBtnSize.x) * 0.5f);
        auto& settingsSlice = atlas->getSlice("settings_btn");
        ImVec2 settingsUv0(settingsSlice.uvRect.x, 1.0f - settingsSlice.uvRect.y);
        ImVec2 settingsUv1(settingsSlice.uvRect.x + settingsSlice.uvRect.z, 1.0f - (settingsSlice.uvRect.y + settingsSlice.uvRect.w));
        bool settingsPressed = ImGui::ImageButton("##settingsbtn", atlasTexId, settingsBtnSize, settingsUv0, settingsUv1);

        min = ImGui::GetItemRectMin();
        max = ImGui::GetItemRectMax();
        if (ImGui::IsItemActive())
            ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(255, 0, 0, 200), 10.0f);
        else if (ImGui::IsItemHovered())
            ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(0, 255, 0, 200), 10.0f);

        // handle settingsPressed if needed
        ImGui::Spacing();

        // ----- EXIT BUTTON -----
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - exitBtnSize.x) * 0.5f);
        auto& exitSlice = atlas->getSlice("exit_btn");
        ImVec2 exitUv0(exitSlice.uvRect.x, 1.0f - exitSlice.uvRect.y);
        ImVec2 exitUv1(exitSlice.uvRect.x + exitSlice.uvRect.z, 1.0f - (exitSlice.uvRect.y + exitSlice.uvRect.w));
        bool exitPressed = ImGui::ImageButton("##exitbtn", atlasTexId, exitBtnSize, exitUv0, exitUv1);

        min = ImGui::GetItemRectMin();
        max = ImGui::GetItemRectMax();
        if (ImGui::IsItemActive())
            ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(255, 0, 0, 200), 10.0f);
        else if (ImGui::IsItemHovered())
            ImGui::GetWindowDrawList()->AddRectFilled(min, max, IM_COL32(0, 255, 0, 200), 10.0f);

        if (exitPressed) {
            glfwSetWindowShouldClose(engine->getWindow(), GLFW_TRUE);
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Version 1.0").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Version 1.0");

        ImGui::End();
    }


    void MainMenu::render()
    {
        // drawButton(playBtn);
        // drawButton(settingsBtn);
        // drawButton(exitBtn);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

} // namespace Chained
