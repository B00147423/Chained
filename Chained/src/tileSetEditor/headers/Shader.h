#pragma once

#include <unordered_map>
#include <string>
#include "Texture2D.h"
#include "Shader.h"
#include "glad/glad.h"
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <map>
namespace Chained{

	class Shader {
	public:

		Shader();
		Shader(const std::string& vertSource, const std::string& fragSource);
		~Shader();

		bool attachShaderSource(GLenum shaderType, const std::string& shaderSource, std::string* log = nullptr);
		bool attachShaderFile(GLenum shaderType, const std::string& shaderFilePath, std::string* log = nullptr);
		bool compile(std::string* log = nullptr);
		Shader* use();
		Shader* unuse();


	public:
		bool setUniform(const std::string& name, GLuint value, bool bUseShader = false);
		bool setUniform(const std::string& name, GLint value, bool bUseShader = false);
		bool setUniform(const std::string& name, GLfloat value, bool bUseShader = false);
		bool setUniform(const std::string& name, GLdouble value, bool bUseShader = false);

		bool setUniform(const std::string& name, glm::vec2 vec, bool bUseShader = false);
		bool setUniform(const std::string& name, glm::vec3 vec, bool bUseShader = false);
		bool setUniform(const std::string& name, glm::vec4 vec, bool bUseShader = false);
		bool setUniform(const std::string& name, glm::mat4 mat4, bool bUseShader = false);
	
		void clearShaders();

	private:
		GLuint m_program = 0;
		std::map<GLenum, GLuint> m_shaderMap;
	};
}