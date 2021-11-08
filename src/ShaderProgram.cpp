#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "ShaderProgram.h"

void PAG::ShaderProgram::CheckProgramCompileStatus()
{
	GLint linkResult = 0;
	glGetProgramiv(idSP, GL_LINK_STATUS, &linkResult);
	if (linkResult == GL_FALSE)
	{
		// Ha habido un error y hay que recuperar su descripción, para saber qué ha pasado
		GLint msgSize = 0;
		std::string message = "";
		glGetProgramiv(idSP, GL_INFO_LOG_LENGTH, &msgSize);
		if (msgSize > 0)
		{
			GLchar* message_c_str = new GLchar[msgSize];
			GLint writtenData = 0;
			glGetProgramInfoLog(idSP, msgSize, &writtenData, message_c_str);
			message.assign(message_c_str);
			delete[] message_c_str;
			message_c_str = nullptr;

			throw std::runtime_error("ShaderProgram::CheckProgramCompileStatus() -> Error al enlazar Shader Program: " + message);
		}
	}
}

PAG::ShaderProgram::ShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile) : vertexShader(nullptr), fragmentShader(nullptr)
{
	try {
		vertexShader = new ShaderObject(ShaderType::VERTEX_SHADER, vertexShaderFile);
		fragmentShader = new ShaderObject(ShaderType::FRAGMENT_SHADER, fragmentShaderFile);
	}
	catch (std::exception& ex) {
		throw std::runtime_error("ShaderProgram::ShaderProgram() -> " + std::string(ex.what()));
	}

	idSP = glCreateProgram();

	if (vertexShader != nullptr) {
		GLuint idVS = vertexShader->GetID();
		glAttachShader(idSP, idVS);
	}

	if (fragmentShader != nullptr) {
		GLuint idFS = fragmentShader->GetID();
		glAttachShader(idSP, idFS);
	}

	glLinkProgram(idSP);

	try {
		CheckProgramCompileStatus();
	}
	catch (std::exception& ex) {
		throw std::runtime_error("ShaderProgram::ShaderProgram() -> " + std::string(ex.what()));
	}
}

PAG::ShaderProgram::ShaderProgram(const ShaderProgram& other)
{
	try {
		vertexShader = new ShaderObject(ShaderType::VERTEX_SHADER, other.vertexShader->GetFileName());
		fragmentShader = new ShaderObject(ShaderType::FRAGMENT_SHADER, other.fragmentShader->GetFileName());
	}
	catch (std::exception& ex) {
		throw std::runtime_error("ShaderProgram::ShaderProgram() -> " + std::string(ex.what()));
	}

	idSP = glCreateProgram();

	if (vertexShader != nullptr) {
		GLuint idVS = vertexShader->GetID();
		glAttachShader(idSP, idVS);
	}

	if (fragmentShader != nullptr) {
		GLuint idFS = fragmentShader->GetID();
		glAttachShader(idSP, idFS);
	}

	glLinkProgram(idSP);

	try {
		CheckProgramCompileStatus();
	}
	catch (std::exception& ex) {
		throw std::runtime_error("ShaderProgram::CopyShaderProgram() -> " + std::string(ex.what()));
	}
}

PAG::ShaderProgram::~ShaderProgram()
{
	delete vertexShader;
	delete fragmentShader;

	glDeleteProgram(idSP);

	vertexShader, fragmentShader = nullptr;
}

GLuint PAG::ShaderProgram::GetID()
{
	return idSP;
}

