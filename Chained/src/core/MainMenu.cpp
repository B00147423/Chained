#include "../headers/MainMenu.h"
#include "../headers/resourceManager.h"
#include "../headers/TestState.h"
#include "../headers/Engine.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
namespace Chained {

    MainMenu::MainMenu(Engine* eng) : engine(eng) {}

    void MainMenu::onEnter() {
        auto& rm = *ResourceManager::get();
        rm.addSearchPath("assets/shaders");
        rm.addSearchPath("assets/textures");

        auto shader = rm.loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
        renderer = std::make_shared<SpriteRenderer>(shader);

        glm::mat4 projection = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);
        shader->use();
        shader->setUniform("projection", projection);
        shader->setUniform("image", 0);

        atlas = std::make_unique<SpriteAtlas>("assets/textures/sprites.json");
    }

    void MainMenu::onExit() {}

    void MainMenu::update(float dt) {
        if (glfwGetMouseButton(engine->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double x, y;
            glfwGetCursorPos(engine->getWindow(), &x, &y);

            if (x >= playButtonPos.x && x <= playButtonPos.x + playButtonSize.x &&
                y >= playButtonPos.y && y <= playButtonPos.y + playButtonSize.y) {
                engine->run(std::make_unique<TestState>());  // Switch to test state
            }
        }
    }

    void MainMenu::render() {
        auto& playBtn = atlas->getSlice("play_button");
        auto uv = playBtn.uvRect;
        uv.y = 1.0f - uv.y - uv.w;

        renderer->DrawSprite(
            atlas->getTexture(),
            playButtonPos,
            playButtonSize,
            0.0f,
            glm::vec3(1.0f),
            uv
        );
    }

}
