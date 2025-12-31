#pragma once
#include "../headers/spriteRenderer.h"
#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


namespace Chained {

	SpriteRenderer::SpriteRenderer(ShaderPtr shader) {
		m_shader = shader;
        std::cout << "[DEBUG] SpriteRenderer constructor entered\n";
		this->initRenderData();
      
        std::cout << "[DEBUG] initRenderData() finished\n";
	}
	//
	SpriteRenderer::~SpriteRenderer()
	{
		glDeleteVertexArrays(1, &m_quadVAO);
	}
    void SpriteRenderer::DrawSprite(Texture2DPtr texture, glm::vec2 position, glm::vec2 size, GLfloat rotate,glm::vec3 color, glm::vec4 uvRect)
    {
        // Prepare transformations
        m_shader->use();
        m_shader->setUniform("uvRect", uvRect);
        glm::mat4 model(1.0f);

        // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
        model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

        model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

        m_shader->setUniform("model", model);

        // Render textured quad
        m_shader->setUniform("spriteColor", color);

       
        texture->bind();

        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    void SpriteRenderer::initRenderData()
    {
        GLuint VBO;
        // Correct quad vertices
        GLfloat vertices[] = {
            // Pos      // Tex
            0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.0f, 1.0f, 0.0f, 1.0f, // top-left

            1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, 1.0f, 1.0f, // top-right
            0.0f, 1.0f, 0.0f, 1.0f  // top-left
        };
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // POS (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // TEX COORDS (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

}