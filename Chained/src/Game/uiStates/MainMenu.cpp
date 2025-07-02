//#include "MainMenu.h"
//#include "TestState.h"
//#include "../../headers/resourceManager.h"
//#include "../../headers/Engine.h"
//#include "../../headers/EditorState.h"
//#include <GLFW/glfw3.h>
//#include <glm/gtc/matrix_transform.hpp>
//
//namespace Chained {
//
//    // ───────────────────────────────────────────────
//    MainMenu::MainMenu(Engine* eng) : engine(eng) {}
//
//    // ───────────────────────────────────────────────
//    void MainMenu::onEnter()
//    {
//        auto& rm = *ResourceManager::get();
//        rm.addSearchPath("assets/shaders");
//        rm.addSearchPath("assets/textures");
//
//        auto shader = rm.loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
//        renderer = std::make_shared<SpriteRenderer>(shader);
//
//        glm::mat4 projection = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);
//        shader->use();
//        shader->setUniform("projection", projection);
//        shader->setUniform("image", 0);
//
//        atlas = std::make_unique<SpriteAtlas>("assets/textures/sprites.json");
//    }
//
//    void MainMenu::onExit() {}
//
//    // ───────────────────────────────────────────────
//    void MainMenu::update(float dt)
//    {
//        int winW, winH;
//        glfwGetWindowSize(engine->getWindow(), &winW, &winH);
//
//        double mx, my;
//        glfwGetCursorPos(engine->getWindow(), &mx, &my);
//
//        // convert to 800×600 virtual space
//        float px = static_cast<float>(mx) * (800.f / winW);
//        float py = static_cast<float>(my) * (600.f / winH);
//
//        int mouse = glfwGetMouseButton(engine->getWindow(), GLFW_MOUSE_BUTTON_LEFT);
//        if (glfwGetKey(engine->getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
//            engine->run(std::make_unique<EditorState>(engine));
//        }
//        // handle each button
//        handleInput(playBtn, px, py, mouse);
//        handleInput(settingsBtn, px, py, mouse);
//        handleInput(exitBtn, px, py, mouse);
//
//        // if exit clicked → close window
//        if (exitBtn.pressed && mouse == GLFW_RELEASE && exitBtn.hovered)
//        {
//            exitBtn.pressed = false;
//            glfwSetWindowShouldClose(engine->getWindow(), GLFW_TRUE);
//        }
//        // basic scale pulse on *any* press
//        clickScale += ((mouse == GLFW_PRESS ? 0.95f : 1.0f) - clickScale) * 10.f * dt;
//    }
//
//    // ───────────────────────────────────────────────
//    void MainMenu::handleInput(MenuButton& btn,
//        float px, float py,
//        int mouseState)
//    {
//        // hover test
//        btn.hovered =
//            px >= btn.pos.x && px <= btn.pos.x + btn.size.x &&
//            py >= btn.pos.y && py <= btn.pos.y + btn.size.y;
//
//        // press logic
//        if (btn.hovered && mouseState == GLFW_PRESS && !btn.pressed)
//            btn.pressed = true;
//
//        if (mouseState == GLFW_RELEASE && btn.pressed)
//        {
//            btn.pressed = false;
//            if (btn.hovered)
//            {
//                if (btn.sliceName == std::string("play_btn"))
//                    engine->run(std::make_unique<TestState>(
//                        std::vector<Chained::SceneObject>(), // Empty objects vector
//                        glm::vec2(0.0f, 0.0f), // Default camera position
//                        1.0f, // Default zoom
//                        800.0f, // Default viewport width
//                        600.0f  // Default viewport height
//                    ));
//                // TODO: open settings menu when you have it
//            }
//        }
//    }
//
//    // ───────────────────────────────────────────────
//    void MainMenu::drawButton(const MenuButton& btn)
//    {
//        auto& slice = atlas->getSlice(btn.sliceName);
//        glm::vec4 uv = slice.uvRect;
//        uv.y = 1.f - uv.y - uv.w;               // flip Y
//
//        glm::vec3 tint = idleCol;
//        if (btn.pressed)       tint = pressedCol;
//        else if (btn.hovered)  tint = hoverCol;
//
//        // outline
//        if (btn.hovered && !btn.pressed)
//        {
//            glm::vec2 outSize = btn.size + glm::vec2(outlinePx * 2.f);
//            glm::vec2 outPos = btn.pos - glm::vec2(outlinePx);
//            renderer->DrawSprite(atlas->getTexture(), outPos, outSize,
//                0.f, glm::vec3(0.f), uv);
//        }
//
//        renderer->DrawSprite(atlas->getTexture(),
//            btn.pos,
//            btn.size,
//            0.f,
//            tint,
//            uv);
//    }
//
//    // ───────────────────────────────────────────────
//    void MainMenu::render()
//    {
//        drawButton(playBtn);
//        drawButton(settingsBtn);
//        drawButton(exitBtn);
//    }
//
//} // namespace Chained
