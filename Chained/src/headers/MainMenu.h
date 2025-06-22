#pragma once
#include "GameState.h"
#include "spriteRenderer.h"
#include "SpriteAtlas.h"

namespace Chained {

    class Engine; // forward declare if needed

    class MainMenu : public GameState {
    public:
        MainMenu(Engine* engine);  // need engine to switch state

        void onEnter() override;
        void onExit() override;
        void update(float dt) override;
        void render() override;

    private:
        Engine* engine;
        std::shared_ptr<SpriteRenderer> renderer;
        std::unique_ptr<SpriteAtlas> atlas;

        glm::vec2 playButtonPos = { 300, 250 };
        glm::vec2 playButtonSize = { 200, 80 };
    };

}
