#pragma once
#include <string>
#include <sstream>
#include <fstream>

namespace PAG {

	enum class ShaderType {
		VERTEX_SHADER,
		FRAGMENT_SHADER
	};

	class ShaderObject
	{
	private:
		ShaderType type;

		const std::string directory = ".\\shaders\\";
		const std::string filePrefix = "pag10-";
		const std::string fileSufix = ".glsl";

		std::string fileName;
		
		GLuint shaderID;

		std::string LoadShader(std::string name);
		void CompileShader(std::string shaderCode);
		void CheckShaderCompileStatus();

	public:
		ShaderObject(ShaderType shaderType, std::string fileName);
		ShaderObject(const ShaderObject& other);
		virtual ~ShaderObject();
		GLuint GetID();
		std::string GetFileName();
	};
}

