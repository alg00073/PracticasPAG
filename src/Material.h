#pragma once
#include "glm/glm.hpp"
#include "Texture.h"
#include "ShaderProgram.h"

namespace PAG {
	class Material
	{
	private:

		glm::vec3 ambientColor; // Color ambiente
		glm::vec3 diffuseColor; // Color difuso
		glm::vec3 specularColor; // Color especular

		float phongExponent; // Exponente de phong (brillo)

		ShaderProgram* shaderProgram = nullptr;
		ShaderProgram* shaderProgramNormalMap = nullptr;

		Texture* texture = nullptr;
		Texture* textureNormalMap = nullptr;

		bool hasNormalMap = false;

	public:
		Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns);
		Material(const Material& other);
		~Material();

		void SetTexture(Texture* texture);
		Texture* GetTexture();

		void SetNormalMapTexture(Texture* textureNormalMap);
		Texture* GetTextureNormalMap();

		void SetShaderProgram(std::string vertexShader, std::string fragmentShader);
		ShaderProgram* GetShaderProgram();

		void SetShaderProgramNormalMap(std::string vertexShader, std::string fragmentShader);
		ShaderProgram* GetShaderProgramNormalMap();
		bool IsNormalMap();

		glm::vec3 GetAmbientColor();
		glm::vec3 GetDiffuseColor();
		glm::vec3 GetSpecularColor();
		float GetPhongExponent();
	};
}

