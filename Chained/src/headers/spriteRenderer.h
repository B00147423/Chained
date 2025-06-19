#pragma once
#include "../headers/texture2d.h"
#include "../headers/Shader.h"
#include "../headers/types.h"



namespace Chained {
    class SpriteRenderer
    {
    public:
        SpriteRenderer(ShaderPtr shader);
        ~SpriteRenderer();

        void DrawSprite(Texture2DPtr texture, glm::vec2 position,
            glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f));
    

    private:
        ShaderPtr m_shader;
        GLuint m_quadVAO;

        void initRenderData();
    };
}