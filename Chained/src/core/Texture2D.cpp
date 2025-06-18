#include "../headers/Texture2D.h"

namespace Chained {

    Texture2D::Texture2D()
    {
        glGenTextures(1, &m_id);
    }

    Texture2D::~Texture2D()
    {
        glBindTexture(GL_TEXTURE_2D, 0);    // Unbind before deletion (optional but clean)
        glDeleteTextures(1, &m_id);
    }

    void Texture2D::generate(GLuint width, GLuint height, unsigned char* data)
    {
        m_width = width;
        m_height = height;

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, m_GpuTextureFormat, m_width, m_height, 0, m_textureRenderFormat, GL_UNSIGNED_BYTE, data);
        // Texture wrapping (both directions)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);

        // Texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMax);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind after setup
    }

    void Texture2D::bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

} // namespace Chained
