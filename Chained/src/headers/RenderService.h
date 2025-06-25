#pragma once
#include "spriteRenderer.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>

namespace Chained {
    class RenderService {
    public:
        static void init(float screenWidth, float screenHeight);
        static SpriteRenderer* getRenderer();
        static Shader* getShader();
        static glm::mat4 getProjection();
       
        static std::shared_ptr<SpriteRenderer> renderer;
        static std::shared_ptr<Shader> shader;
        static glm::mat4 projection;
    };
}
