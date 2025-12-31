#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../../headers/GameState.h"
#include "../../headers/SpriteAtlas.h"
#include "../../headers/SpriteRenderer.h"
#include "../../headers/ResourceManager.h"
#include "../../headers/Camera.h"
#include "../../headers/types.h"
#include "../../headers/physics.h"

namespace Chained {

    class TestState : public GameState {
    public:
        explicit TestState(const std::string& sceneFile);

        void onEnter() override;
        void onExit() override;
        void update(float dt) override;
        void render() override;

    private:
        void loadSceneFromJson(const std::string& filename);

        std::vector<std::unique_ptr<SceneObject>> objects;
        std::unique_ptr<Camera> camera;
        std::unique_ptr<SpriteAtlas> atlas;
        std::shared_ptr<SpriteRenderer> renderer;
        std::shared_ptr<Shader> shader;

        std::unique_ptr<PhysicsSystem> physics;
    };

}
