#pragma once

#include "../../headers/GameState.h"
#include "../../headers/Engine.h"
#include "../../headers/SpriteAtlas.h"
#include "../../headers/spriteRenderer.h"
#include "../../headers/resourceManager.h"
#include "../../headers/Camera.h"
#include "../../headers/Shader.h"
#include <memory>
#include <vector>
#include <string>

namespace Chained {

    struct Player {
        glm::vec2 position{ 0, 0 };
        float speed = 250.0f;
    };

    class PlayState : public GameState {
    public:
        explicit PlayState(Engine* eng);

        void onEnter() override;
        void onExit() override;
        void update(float dt) override;
        void render() override;

    private:
        Engine* engine;
        std::shared_ptr<SpriteRenderer> renderer;
        std::unique_ptr<SpriteAtlas> atlas;
        std::unique_ptr<Camera> camera;
        std::shared_ptr<Shader> shader;
        Player player;
    };

}
