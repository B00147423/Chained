#include "TestState.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

using json = nlohmann::json;

bool aabbOverlap(const Chained::SceneObject& a, const Chained::SceneObject& b) {
    glm::vec2 aMin = a.position;
    glm::vec2 aMax = a.position + a.physics.size * a.scale;
    glm::vec2 bMin = b.position;
    glm::vec2 bMax = b.position + b.physics.size * b.scale;
    return (aMin.x < bMax.x && aMax.x > bMin.x &&
            aMin.y < bMax.y && aMax.y > bMin.y);
}

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
            if (objJson.contains("physics")) {
                const auto& phys = objJson["physics"];
                obj.physics.enabled = phys.value("enabled", false);
                obj.physics.bodyType = (Chained::BodyType)phys.value("bodyType", 0);
                obj.physics.shapeType = (Chained::ShapeType)phys.value("shapeType", 0);
                obj.physics.gravityScale = phys.value("gravityScale", 1.0f);
                obj.physics.linearDamping = phys.value("linearDamping", 0.0f);
                obj.physics.angularDamping = phys.value("angularDamping", 0.0f);
                obj.physics.fixedRotation = phys.value("fixedRotation", false);
                obj.physics.isSensor = phys.value("isSensor", false);
                obj.physics.material.friction = phys.value("friction", 0.5f);
                obj.physics.material.bounciness = phys.value("bounciness", 0.0f);
                obj.physics.material.density = phys.value("density", 1.0f);
                if (phys.contains("size")) {
                    obj.physics.size = { phys["size"][0].get<float>(), phys["size"][1].get<float>() };
                }
                obj.physics.radius = phys.value("radius", 0.5f);
            }

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
        float moveSpeed = 200.0f; // pixels per second
        for (size_t i = 0; i < objects.size(); ++i) {
            auto& obj = objects[i];
            if (obj.name == "orc_sword") { // or your player name
                glm::vec2 oldPos = obj.position;
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) {
                    obj.position.y -= moveSpeed * dt;
                }
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
                    obj.position.y += moveSpeed * dt;
                }
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) {
                    obj.position.x -= moveSpeed * dt;
                }
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) {
                    obj.position.x += moveSpeed * dt;
                }
                glm::vec2 moveDelta = obj.position - oldPos;
                // Push other objects on collision
                for (size_t j = 0; j < objects.size(); ++j) {
                    if (i == j) continue;
                    auto& other = objects[j];
                    if (aabbOverlap(obj, other)) {
                        other.position += moveDelta;
                    }
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
