// TestState.cpp
#include "TestState.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../headers/RenderService.h"

using namespace Chained;
void TestState::onEnter() {
    atlas = std::make_unique<SpriteAtlas>("assets/textures/sprites.json");
    renderer = std::shared_ptr<SpriteRenderer>(RenderService::getRenderer(), [](SpriteRenderer*) {});
}

void TestState::onExit() {}

void TestState::update(float) {
    // No logic yet
}

void TestState::render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    auto& heart = atlas->getSlice("heart");
    auto uv = heart.uvRect;
    uv.y = 1.0f - uv.y - uv.w;

    renderer->DrawSprite(
        atlas->getTexture(),
        glm::vec2(100, 100),
        glm::vec2(64, 64),
        0.0f,
        glm::vec3(1.0f),
        uv
    );
    
}

bool aabbOverlap(const Chained::SceneObject& a, const Chained::SceneObject& b) {
    glm::vec2 aMin = a.position;
    glm::vec2 aMax = a.position + a.physics.size * a.scale;
    glm::vec2 bMin = b.position;
    glm::vec2 bMax = b.position + b.physics.size * b.scale;
    return (aMin.x < bMax.x && aMax.x > bMin.x &&
            aMin.y < bMax.y && aMax.y > bMin.y);
}
