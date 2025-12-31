#include "TestState.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

using json = nlohmann::json;

namespace Chained {

    TestState::TestState(const std::string& sceneFile)
    {
        loadSceneFromJson(sceneFile);
        physics = std::make_unique<PhysicsSystem>(b2Vec2(0.0f, 9.8f));
        physics->addObjects(objects);
    }

    void TestState::loadSceneFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;

        json j; file >> j;
        objects.clear();
        for (const auto& objJson : j["objects"]) {
            auto obj = std::make_unique<SceneObject>();
            obj->name = objJson["name"].get<std::string>();
            obj->position = { objJson["position"][0].get<float>(), objJson["position"][1].get<float>() };
            obj->rotation = objJson["rotation"].get<float>();
            obj->scale = { objJson["scale"][0].get<float>(), objJson["scale"][1].get<float>() };
            obj->assetId = objJson["assetId"].get<int>();
            if (objJson.contains("physics")) {
                const auto& phys = objJson["physics"];
                obj->physics.enabled = phys.value("enabled", false);
                obj->physics.bodyType = (Chained::BodyType)phys.value("bodyType", 0);
                obj->physics.shapeType = (Chained::ShapeType)phys.value("shapeType", 0);
                obj->physics.gravityScale = phys.value("gravityScale", 1.0f);
                obj->physics.linearDamping = phys.value("linearDamping", 0.0f);
                obj->physics.angularDamping = phys.value("angularDamping", 0.0f);
                obj->physics.fixedRotation = phys.value("fixedRotation", false);
                obj->physics.isSensor = phys.value("isSensor", false);
                obj->physics.material.friction = phys.value("friction", 0.5f);
                obj->physics.material.bounciness = phys.value("bounciness", 0.0f);
                obj->physics.material.density = phys.value("density", 1.0f);
                if (phys.contains("size")) {
                    obj->physics.size = { phys["size"][0].get<float>(), phys["size"][1].get<float>() };
                }
                obj->physics.radius = phys.value("radius", 0.5f);
            }
            objects.push_back(std::move(obj));
        }

        if (j.contains("camera")) {
            auto camJson = j["camera"];
            glm::vec2 camPos = { camJson["pos"][0].get<float>(), camJson["pos"][1].get<float>() };
            float camZoom = camJson["zoom"].get<float>();
            camera = std::make_unique<Camera>(1280.0f, 720.0f);
            camera->setPostion(camPos);
            camera->setZoom(camZoom);
        }
        else {
            camera = std::make_unique<Camera>(1280.0f, 720.0f);
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

    void TestState::onExit() {}

    void TestState::update(float dt) {
        // Apply movement with forces for better pushing
        for (auto& obj : objects) {
            if (obj->name == "orc_sword") {
                b2Body* body = physics->getBodyFor(obj.get());
                if (body) {
                    b2Vec2 vel(0, 0);
                    float speed = 10.0f; // Faster speed for better pushing
                    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) vel.y = -speed;
                    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) vel.y = speed;
                    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) vel.x = -speed;
                    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) vel.x = speed;
                    body->SetLinearVelocity(vel);
                    
                    // Debug: Print velocity and mass
                    static int frameCount = 0;
                    if (frameCount++ % 60 == 0) { // Print every 60 frames
                        std::cout << "Sword velocity: (" << vel.x << ", " << vel.y << ")" << std::endl;
                        std::cout << "Sword mass: " << body->GetMass() << std::endl;
                    }
                }
            }
        }

        physics->step(dt);
        physics->syncToObjects(objects);
        
        // No more manual AABB collision detection - Box2D handles it automatically!
    }

    void TestState::render() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        auto tex = atlas->getTexture();
        if (!tex) return;
        int texW = tex->m_width, texH = tex->m_height;
        for (const auto& obj : objects) {
            if (obj->assetId < 0 || obj->assetId >= static_cast<int>(atlas->getAllSlices().size())) continue;

            const auto& asset = atlas->getSlice(obj->name);
            glm::vec2 size = { asset.uvRect.z * texW * obj->scale.x, asset.uvRect.w * texH * obj->scale.y };
            glm::vec4 uv = asset.uvRect;
            uv.y = 1.0f - uv.y - uv.w;
            renderer->DrawSprite(tex, obj->position, size, obj->rotation, glm::vec3(1.0f), uv);
        }
        
        // Debug: Draw physics collision shapes
        for (const auto& obj : objects) {
            if (!obj->physics.enabled) continue;
            
            b2Body* body = physics->getBodyFor(obj.get());
            if (!body) continue;
            
            // Get body position and transform
            b2Vec2 pos = body->GetPosition();
            float angle = body->GetAngle();
            
            // Convert from Box2D meters to screen pixels
            glm::vec2 screenPos = { pos.x * PhysicsSystem::PHYSICS_SCALE, pos.y * PhysicsSystem::PHYSICS_SCALE };
            
            if (obj->physics.shapeType == ShapeType::Box) {
                // Draw box collision shape
                glm::vec2 size = obj->physics.size * obj->scale;
                glm::vec2 halfSize = size * 0.5f;
                
                // Calculate corners in local space
                glm::vec2 corners[4] = {
                    {-halfSize.x, -halfSize.y},
                    {+halfSize.x, -halfSize.y},
                    {+halfSize.x, +halfSize.y},
                    {-halfSize.x, +halfSize.y}
                };
                
                // Transform corners to world space
                float c = cos(angle), s = sin(angle);
                glm::vec2 worldCorners[4];
                for (int i = 0; i < 4; ++i) {
                    worldCorners[i].x = screenPos.x + (corners[i].x * c - corners[i].y * s);
                    worldCorners[i].y = screenPos.y + (corners[i].x * s + corners[i].y * c);
                }
                
                // Draw collision box outline (red)
                for (int i = 0; i < 4; ++i) {
                    // Simple line drawing - you might want to use your debug drawing system
                    // DrawDebugLine(worldCorners[i], worldCorners[(i + 1) % 4], glm::vec3(1, 0, 0));
                }
            }
        }
    }

}
