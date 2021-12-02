#pragma once

#include "ShaderObject.h"
#include "glm/glm.hpp"

namespace PAG {

	class ShaderProgram
	{
	private:
		GLuint idSP = 0; // Identificador del shader program

		ShaderObject* vertexShader;
		ShaderObject* fragmentShader;

		void CheckProgramCompileStatus();

	public:
		ShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile);
		ShaderProgram(const ShaderProgram &other);
		virtual ~ShaderProgram();
		GLuint GetID();

		void SetUniform1f(std::string name, float data);
		void SetUniform3fv(std::string name, glm::vec3 data);
		void SetUniform4fm(std::string name, glm::mat4 data);
		void SetUniformSampler2D(std::string name, GLint sampler);
		void SetSubroutineUniforms(std::string subroutineName1, std::string subroutineName2);
	};
}

