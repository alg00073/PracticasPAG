#pragma once

#include "ShaderObject.h"

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
	};
}

