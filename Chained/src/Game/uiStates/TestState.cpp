#include "TestState.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

using json = nlohmann::json;

namespace Chained {

    TestState::TestState(const std::string& sceneFile) {
        loadSceneFromJson(sceneFile);
    }

    void TestState::loadSceneFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Could not open scene file: " << filename << std::endl;
            return;
        }

        json j;
        file >> j;

        objects.clear();
        for (const auto& objJson : j["objects"]) {
            SceneObject obj;
            obj.name = objJson["name"].get<std::string>();
            obj.position = { objJson["position"][0].get<float>(), objJson["position"][1].get<float>() };
            obj.rotation = objJson["rotation"].get<float>();
            obj.scale = { objJson["scale"][0].get<float>(), objJson["scale"][1].get<float>() };
            obj.assetId = objJson["assetId"].get<int>();
            objects.push_back(obj);
        }

        if (j.contains("camera")) {
            auto camJson = j["camera"];
            glm::vec2 camPos = { camJson["pos"][0].get<float>(), camJson["pos"][1].get<float>() };
            float camZoom = camJson["zoom"].get<float>();
            camera = std::make_unique<Camera>(1280.0f, 720.0f); // Use default or hardcoded values
            camera->setPostion(camPos);
            camera->setZoom(camZoom);
        }
        else {
            camera = std::make_unique<Camera>(1280.0f, 720.0f); // Use default or hardcoded values
        }


    }

    void TestState::onEnter() {
        auto& rm = *ResourceManager::get();

        atlas = std::make_unique<SpriteAtlas>("assets/textures/sprites.json");
        shader = rm.loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
        renderer = std::make_shared<SpriteRenderer>(shader);

        shader->use();
        shader->setUniform("projection", camera->getProjectionMatrix());
        shader->setUniform("image", 0);
    }

    void TestState::onExit() {
        // Cleanup if needed
    }

    void TestState::update(float dt) {
        // Basic WASD movement for a specific player asset
        float moveSpeed = 200.0f; // pixels per second
        
        // Find the player object by name (you can change "player" to whatever you want)
        for (auto& obj : objects) {
            if (obj.name == "mallet" || obj.name == "mallet") {
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) {
                    obj.position.y += moveSpeed * dt;
                }
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
                    obj.position.y += moveSpeed * dt;
                }
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) {
                    obj.position.x -= moveSpeed * dt;
                }
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) {
                    obj.position.x -= moveSpeed * dt;
                }
                break; // Only move the first player found
            }
        }
    }

    void TestState::render() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        auto tex = atlas->getTexture();
        if (!tex) return;
        int texW = tex->m_width;
        int texH = tex->m_height;

        for (const auto& obj : objects) {
            if (obj.assetId < 0 || obj.assetId >= static_cast<int>(atlas->getAllSlices().size())) continue;
            const auto& asset = atlas->getSlice(obj.name);
            glm::vec2 size = {
                asset.uvRect.z * texW * obj.scale.x,
                asset.uvRect.w * texH * obj.scale.y
            };
            glm::vec4 uv = asset.uvRect;
            uv.y = 1.0f - uv.y - uv.w;

            renderer->DrawSprite(
                tex,
                obj.position,
                size,
                obj.rotation,
                glm::vec3(1.0f),
                uv
            );
        }
    }

} // namespace Chained
