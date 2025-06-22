// TestState.cpp
#include "../headers/TestState.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace Chained;
void TestState::onEnter() {
    using namespace Chained;
    ResourceManager::get()->addSearchPath("assets/shaders");
    ResourceManager::get()->addSearchPath("assets/textures");

    auto shader = ResourceManager::get()->loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
    renderer = std::make_shared<SpriteRenderer>(shader);

    glm::mat4 projection = glm::ortho(0.0f, 800.f, 600.f, 0.f, -1.0f, 1.0f);
    shader->use();
    shader->setUniform("projection", projection);
    shader->setUniform("image", 0);

    atlas = std::make_unique<SpriteAtlas>("assets/textures/sprites.json");
}

void TestState::onExit() {
    // Cleanup if needed
}

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
        glm::vec2(uv.z * 1000, uv.w * 1051),
        0.0f,
        glm::vec3(1.0f),
        uv
    );
}
