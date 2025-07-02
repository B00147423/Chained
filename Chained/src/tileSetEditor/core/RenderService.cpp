#include "../headers/RenderService.h"
#include "../headers/resourceManager.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Chained {

    std::shared_ptr<SpriteRenderer> RenderService::renderer = nullptr;
    std::shared_ptr<Shader> RenderService::shader = nullptr;
    glm::mat4 RenderService::projection = glm::mat4(1.0f);

    void RenderService::init(float screenWidth, float screenHeight) {
        auto& rm = *ResourceManager::get();
        rm.addSearchPath("assets/shaders");
        rm.addSearchPath("assets/textures");

        shader = rm.loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
        shader->use();

        projection = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
        shader->setUniform("projection", projection);
        shader->setUniform("image", 0);

        renderer = std::make_shared<SpriteRenderer>(shader);
    }

    SpriteRenderer* RenderService::getRenderer() {
        return renderer.get();
    }

    Shader* RenderService::getShader() {
        return shader.get();
    }

    glm::mat4 RenderService::getProjection() {
        return projection;
    }

}
