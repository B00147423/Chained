#define STB_IMAGE_IMPLEMENTATION
#include "../headers/stb_image.h"
#include "../headers/resourceManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>



namespace Chained {

	ResourceManager* ResourceManager::m_instance = nullptr;

	ResourceManager::ResourceManager()
	{
		//stbi_set_flip_vertically_on_load(true);
	}

	std::shared_ptr<Shader> ResourceManager::loadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* pShaderFile, const std::string& name)
	{
		m_shaderMap[name] = std::shared_ptr<Shader>(loadShaderFromFile(vShaderFile, fShaderFile, pShaderFile));
		return m_shaderMap[name];
	}
	std::shared_ptr<Shader> ResourceManager::getShader(const std::string& name)
	{
		if (m_shaderMap.find(name) == m_shaderMap.end())
			return std::shared_ptr<Shader>(nullptr);
		return m_shaderMap[name];
	}


	Texture2DPtr ResourceManager::loadTexture(const GLchar* file, GLboolean alpha, const std::string& name)
	{
		m_textureMap[name] = std::shared_ptr<Texture2D>(loadTextureFromFile(file, alpha));
		return m_textureMap[name];
	}

	Texture2DPtr ResourceManager::getTexture(const std::string& name) {
		if (m_textureMap.find(name) == m_textureMap.end()) {
			return nullptr;
		}
		return m_textureMap[name];
	}



	void ResourceManager::clear()
	{
		m_shaderMap.clear();
		m_textureMap.clear();
	}

	Shader* ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile)
	{
		std::string vertexCode;
		std::string fragmentCode;

		try {
			std::string vShaderFilePath = solveResourcePath(vShaderFile);
			std::string fShaderFilePath = solveResourcePath(fShaderFile);

			std::ifstream vertexShaderFile(vShaderFilePath);

			if (!vertexShaderFile.is_open()) {
				std::cerr << "ERROR::SHADER: Failed to open vertex shader file: " << vShaderFilePath << std::endl;
				return nullptr;
			}
			std::cout << "Successfully opened vertex shader: " << vShaderFilePath << std::endl;

			std::ifstream fragmentShaderFile(fShaderFilePath);

			if (!fragmentShaderFile.is_open()) {
				std::cerr << "ERROR::SHADER: Failed to open fragment shader file: " << fShaderFilePath << std::endl;
				return nullptr;
			}
			std::cout << "Successfully opened fragment shader: " << fShaderFilePath << std::endl;

			std::stringstream vShaderStream, fShaderStream;

			// Read file's buffer contents into streams
			// This copies the whole file into the stringstream
			vShaderStream << vertexShaderFile.rdbuf();
			fShaderStream << fragmentShaderFile.rdbuf();

			// Get a string version of the file content

			// close file handlers
			vertexShaderFile.close();
			fragmentShaderFile.close();


			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();



			if (vertexCode.empty()) {
				std::cerr << "ERROR::SHADER: Vertex shader file is empty: " << vShaderFilePath << std::endl;
				return nullptr;
			}

			if (fragmentCode.empty()) {
				std::cerr << "ERROR::SHADER: Vertex shader file is empty: " << vShaderFilePath << std::endl;
				return nullptr;
			}

		}
		catch (std::exception e) {
			std::cerr << "ERROR::SHADER: Failed to read shader files: " << e.what() << std::endl;
			return nullptr;
		}

		// 2. Now create shader object from source code
		Shader* shader = new Shader();
		std::string log;
		if (!shader->attachShaderSource(GL_VERTEX_SHADER, vertexCode, &log)) {
			std::cerr << "ERROR::SHADER: Failed to compile vertex shader:\n" << log << std::endl;
			delete shader;
			return nullptr;
		}
		if (!shader->attachShaderSource(GL_FRAGMENT_SHADER, fragmentCode, &log)) {
			std::cerr << "ERROR::SHADER: Failed to compile fragment shader:\n" << log << std::endl;
			delete shader;
			return nullptr;
		}
		if (!shader->compile(&log)) {
			std::cerr << "ERROR::SHADER: Failed to link shader program:\n" << log << std::endl;
			delete shader;
			return nullptr;
		}
		return shader;
	}

	Texture2D* ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha) {
		std::cout << "[DEBUG] [loadTextureFromFile] Begin" << std::endl;

		Texture2D* texture = new Texture2D();

		int width = 0, height = 0, nrChannels = 0;

		std::string filePath = solveResourcePath(file);
		std::cout << "[DEBUG] Resolved texture path: " << filePath << std::endl;

		unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 4);
		std::cout << "[DEBUG] Loaded image: " << width << "x" << height << " channels: " << nrChannels << std::endl;
		if (!image) {
			std::cerr << "[ERROR] Failed to load texture from file: " << file << std::endl;
			delete texture;
			return nullptr;
		}

		std::cout << "[DEBUG] Loaded image: " << width << "x" << height << " channels: " << nrChannels << std::endl;

		// Set the correct format based on the number of channels
// Warn if user asked for alpha but image doesn't have 4 channels
		if (alpha && nrChannels < 4) {
			std::cerr << "⚠️ Alpha requested but image only has " << nrChannels << " channels. Falling back to RGB.\n";
			alpha = false;
		}

		// Set formats based on actual data
		if (alpha && nrChannels == 4) {
			texture->m_GpuTextureFormat = GL_RGBA;
			texture->m_textureRenderFormat = GL_RGBA;
			std::cout << "[DEBUG] Using RGBA format: GPU=" << GL_RGBA << ", Render=" << GL_RGBA << "\n";
		}
		else if (nrChannels == 3) {
			texture->m_GpuTextureFormat = GL_RGB;
			texture->m_textureRenderFormat = GL_RGB;
			std::cout << "[DEBUG] Using RGB format: GPU=" << GL_RGB << ", Render=" << GL_RGB << "\n";
		}
		else if (nrChannels == 1) {
			texture->m_GpuTextureFormat = GL_RED;
			texture->m_textureRenderFormat = GL_RED;
			std::cout << "[DEBUG] Using RED format: GPU=" << GL_RED << ", Render=" << GL_RED << "\n";
		}
		else {
			std::cerr << "[ERROR] Unsupported channel count: " << nrChannels << "\n";
			delete texture;
			stbi_image_free(image);
			return nullptr;
		}


		texture->generate(width, height, image);
		std::cout << "[DEBUG] Finished texture->generate()" << std::endl;
		std::cout << "[DEBUG] Freeing image at pointer: " << static_cast<void*>(image) << std::endl;
		stbi_image_free(image);

		return texture;
	}


	void ResourceManager::setSearchPath(const std::vector<std::string>& paths)
	{
		for (const auto& path : paths)
		{
			m_searchPath.emplace_back(path);
		}
	}
	void ResourceManager::addSearchPath(const std::string& path)
	{
		m_searchPath.emplace_back(path);
	}


	std::string ResourceManager::solveResourcePath(const std::string& path)
	{
		std::ifstream testFile(path);
		if (testFile.good()) {
			testFile.close();
			return path;
		}

		for (const auto& searchPath : m_searchPath) {
			std::string fullPath = searchPath + "/" + path;
			std::ifstream testFile2(fullPath);
			if (testFile2.good()) {
				testFile2.close();
				return fullPath;
			}
		}

		// If not found, return the original path
		std::cerr << "WARNING: Resource not found: " << path << std::endl;
		return path;
	}


}
