// TestState.cpp
#include "../headers/TestState.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../headers/RenderService.h"

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
