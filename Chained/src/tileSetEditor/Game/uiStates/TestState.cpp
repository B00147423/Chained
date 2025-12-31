// TestState.cpp
#include "TestState.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../headers/RenderService.h"

using namespace Chained;
void TestState::onEnter() {
    atlas = std::make_unique<SpriteAtlas>("assets/textures/sprites.json");
    renderer = RenderService::renderer;
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

// Removed manual AABB collision detection - use Box2D physics instead!
