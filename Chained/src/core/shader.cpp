#include "../headers/Shader.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace Chained {
    Shader::Shader()
    {
        m_program = glCreateProgram();
    }

    Shader::Shader(const std::string& vertSource, const std::string& fragSource):Shader()
    {
        assert(attachShaderSource(GL_VERTEX_SHADER, vertSource));
        assert(attachShaderSource(GL_FRAGMENT_SHADER, fragSource));
        assert(compile());
    }
    Shader::~Shader() 
    {
        clearShaders();
        if (m_program) {
            unuse();
            glDeleteProgram(m_program);
            m_program = 0;
        }
    }
    // Loops through every shader in the map:
    // pair.first is the shader type (GL_VERTEX_SHADER, etc.)
    // pair.second is the shader object ID (GLuint) returned by glCreateShader
    // Deletes each shader by ID
    void Shader::clearShaders() {
        for (const auto& pair : m_shaderMap) {
            if(pair.second) {
                glDeleteShader(pair.second);
                }
        }
        m_shaderMap.clear();
    }

    // Loads and attaches a shader from source code in memory.
    // 
    // Parameters:
    // - shaderType: GLenum identifying the shader type (e.g., GL_VERTEX_SHADER).
    // - shaderSource: The GLSL source code as a string.
    // - log: Optional pointer to a string to store error messages.
    //
    // Returns true if the shader compiled and attached successfully,
    // false if compilation failed or the shader type was already added.
    bool Shader::attachShaderSource(GLenum shaderType, const std::string& shaderSource, std::string* log)
    {
        if (m_shaderMap.find(shaderType) != m_shaderMap.end()) {
            std::cerr << "shader type already added !";
            return false;
        }
        int success;
        auto shaderId = glCreateShader(shaderType);
        const char* shaderStr = shaderSource.c_str();
        glShaderSource(shaderId, 1, &shaderStr, nullptr);
        glCompileShader(shaderId);
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            int iLen;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &iLen);
            std::string infoLog;
            infoLog.resize(iLen);
            glGetShaderInfoLog(shaderId, iLen, nullptr, infoLog.data());
            glDeleteShader(shaderId);
            return false;
        }
        glAttachShader(m_program, shaderId);
        m_shaderMap[shaderType] = shaderId;
        return true;
    }

    // Loads a shader from a file and attaches it to the shader program.
    //
    // Parameters:
    // - shaderType: GLenum identifying the shader type (e.g., GL_FRAGMENT_SHADER).
    // - shaderFilePath: Path to the shader source file.
    // - log: Optional pointer to a string to store error messages.
    //
    // Returns true if the file was read and the shader was attached successfully,
    // false if the file couldn't be read, was empty, or compilation failed.

    bool Shader::attachShaderFile(GLenum shaderType, const std::string& shaderFilePath, std::string* log)
    {
        std::ifstream fin(shaderFilePath);
        if (!fin) {
            std::cerr << "Error: Shader file not found: " << shaderFilePath << "\n";
            return false;
        }
        std::stringstream ss;
        ss << fin.rdbuf();
        std::string strShader(ss.str());
        if (!strShader.length()) {
            return false;
        }
        return attachShaderSource(shaderType, strShader, log);
    }
    ///////////////////////////////////////////////////////////////////////////////

    bool Shader::setUniform(const std::string& name, GLuint value, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniform1ui(location, value);
        return true;
    }

    bool Shader::setUniform(const std::string& name, GLint value, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniform1i(location, value);
        return true;
    }

    bool Shader::setUniform(const std::string& name, GLfloat value, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniform1f(location, value);
        return true;
    }

    bool Shader::setUniform(const std::string& name, GLdouble value, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniform1d(location, value);
        return true;
    }

    bool Shader::setUniform(const std::string& name, glm::vec3 vec, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniform3f(location, vec.x, vec.y, vec.z);
        return true;
    }

    bool Shader::setUniform(const std::string& name, glm::vec4 vec, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
        return true;
    }

    bool Shader::setUniform(const std::string& name, glm::vec2 vec, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniform2f(location, vec.x, vec.y);
        return true;
    }

    bool Shader::setUniform(const std::string& name, glm::mat4 mat4, bool bUseShader)
    {
        if (bUseShader)
            use();
        auto location = glGetUniformLocation(m_program, name.c_str());
        if (-1 == location) {
            return false;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
        return true;
    }

    Shader* Shader::use()
    {
        assert(m_program);
        glUseProgram(m_program);
        return this;
    }

    Shader* Shader::unuse()
    {
        glUseProgram(0);
        return this;
    }
    
    bool Shader::compile(std::string* log)
    {
        GLint success;
        glLinkProgram(m_program);
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if (!success) {
            int iLen = 0;
            glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &iLen);
            std::string infoLog;
            infoLog.resize(iLen);
            glGetProgramInfoLog(m_program, iLen, nullptr, infoLog.data());
            if (log) {
                *log = infoLog;
            }
            else {
                std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            clearShaders();
            return false;
        }
        clearShaders();
        return true;
    }

}

