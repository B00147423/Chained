#pragma once
#include <memory>         
#include "glad/glad.h"

namespace Chained {

    class Texture2D
    {
    public:
        GLuint m_id;
        GLuint m_width;
        GLuint m_height;
        GLuint m_GpuTextureFormat = GL_RGBA;
        GLuint m_textureRenderFormat = GL_RGBA;

        GLuint m_wrapS = GL_REPEAT;
        GLuint m_wrapT = GL_REPEAT;
        GLuint m_filterMin = GL_NEAREST;
        GLuint m_filterMax = GL_NEAREST;

    public:
        Texture2D();
        ~Texture2D();
        void generate(GLuint width, GLuint height, unsigned char* data);
        void bind() const;

        // Static helper for 1x1 color texture
        static std::shared_ptr<Texture2D> Create(int width, int height, uint32_t rgba);
    };

} // namespace Chained
