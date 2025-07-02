#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp> // For JSON parsing
#include "../../headers/GameState.h"
#include "../../headers/SpriteAtlas.h"
#include "../../headers/SpriteRenderer.h"
#include "../../headers/ResourceManager.h"
#include "../../headers/Camera.h"
#include "../../headers/types.h"

namespace Chained {

    class TestState : public GameState {
    public:
        // Construct TestState from JSON scene file
        explicit TestState(const std::string& sceneFile);

        void onEnter() override;
        void onExit() override;
        void update(float dt) override;
        void render() override;

    private:
        void loadSceneFromJson(const std::string& filename);

        std::vector<SceneObject> objects;
        std::unique_ptr<Camera> camera;
        std::unique_ptr<SpriteAtlas> atlas;
        std::shared_ptr<SpriteRenderer> renderer;
        std::shared_ptr<Shader> shader;

    private:
        int playerIndex = -1;  // Index of the movable player object in objects vector
        glm::vec2 playerVelocity = glm::vec2(0.0f);
        float moveSpeed = 200.0f; // pixels per second (adjust as needed)
    };

} // namespace Chained