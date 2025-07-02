#include "../headers/Texture2D.h"
#include "iostream"
#include <memory>
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
    std::shared_ptr<Texture2D> Texture2D::Create(int width, int height, uint32_t rgba)
    {
        auto tex = std::make_shared<Texture2D>();
        tex->generate(width, height, reinterpret_cast<unsigned char*>(&rgba));
        return tex;
    }
    void Texture2D::generate(GLuint width, GLuint height, unsigned char* data)
    {
        m_width = width;
        m_height = height;



        glBindTexture(GL_TEXTURE_2D, m_id);
        std::cout << "[DEBUG] glBindTexture done\n";

        std::cout << "[DEBUG] About to call glTexImage2D\n";
        glTexImage2D(GL_TEXTURE_2D, 0, m_GpuTextureFormat, m_width, m_height, 0, m_textureRenderFormat, GL_UNSIGNED_BYTE, data);
        std::cout << "[DEBUG] glTexImage2D done\n";

        std::cout << "[DEBUG] About to set texture parameters\n";
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
        std::cout << "[DEBUG] GL_TEXTURE_WRAP_S set\n";
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
        std::cout << "[DEBUG] GL_TEXTURE_WRAP_T set\n";
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMin);
        std::cout << "[DEBUG] GL_TEXTURE_MIN_FILTER set\n";
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMax);
        std::cout << "[DEBUG] GL_TEXTURE_MAG_FILTER set\n";

        std::cout << "[DEBUG] glTexParameteri done\n";

        glBindTexture(GL_TEXTURE_2D, 0);
        std::cout << "[DEBUG] Texture generation finished\n";
    }


    void Texture2D::bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

} // namespace Chained
