#pragma once
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
};

} // namespace Chained
