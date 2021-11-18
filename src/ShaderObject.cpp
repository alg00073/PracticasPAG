#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "ShaderObject.h"

PAG::ShaderObject::ShaderObject(ShaderType shaderType, std::string fileName): type(shaderType), shaderID(0), fileName(fileName)
{
	std::string shaderCode;

	try {
		shaderCode = LoadShader(fileName);
	}
	catch (std::exception &ex) {
		throw std::runtime_error("ShaderObject::ShaderObject() -> " + std::string(ex.what()));
	}

	if (type == ShaderType::VERTEX_SHADER) {
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	}
	else if(type == ShaderType::FRAGMENT_SHADER) {
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	}

	if (shaderID == 0) {
		throw std::runtime_error("ShaderObject::ShaderObject() -> No se pudo vincular el Shader correctamente");
	}

	CompileShader(shaderCode);

	try {
		CheckShaderCompileStatus();
	}
	catch (std::exception &ex) {
		throw std::runtime_error("ShaderObject::ShaderObject() -> " + std::string(ex.what()));
	}
}

PAG::ShaderObject::ShaderObject(const ShaderObject& other)
{
	fileName = other.fileName;

	std::string shaderCode;

	try {
		shaderCode = LoadShader(fileName);
	}
	catch (std::exception &ex) {
		throw std::runtime_error("ShaderObject::CopyShaderObject() -> " + std::string(ex.what()));
	}

	if (type == ShaderType::VERTEX_SHADER) {
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == ShaderType::FRAGMENT_SHADER) {
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	}

	if (shaderID == 0) {
		throw std::runtime_error("ShaderObject::CopyShaderObject() -> No se pudo vincular el Shader correctamente");
	}

	CompileShader(shaderCode);

	try {
		CheckShaderCompileStatus();
	}
	catch (std::exception &ex) {
		throw std::runtime_error("ShaderObject::CopyShaderObject() -> " + std::string(ex.what()));
	}
}

PAG::ShaderObject::~ShaderObject()
{
	glDeleteShader(shaderID);
}

GLuint PAG::ShaderObject::GetID()
{
	return shaderID;
}

std::string PAG::ShaderObject::GetFileName()
{
	return fileName;
}

std::string PAG::ShaderObject::LoadShader(std::string name)
{
	std::ifstream shaderFile;
	shaderFile.open(filePrefix + name + fileSufix);

	if (!shaderFile.is_open())
	{
		throw std::runtime_error("ShaderObject::LoadShader() -> No se pudo abrir el fichero: " + filePrefix + name + fileSufix);
	}

	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	std::string shaderCode = shaderStream.str();

	shaderFile.close();

	return shaderCode;
}

void PAG::ShaderObject::CompileShader(std::string shaderCode)
{
	const GLchar* shaderCode_c_str = shaderCode.c_str();
	glShaderSource(shaderID, 1, &shaderCode_c_str, nullptr);
	glCompileShader(shaderID);
}

void PAG::ShaderObject::CheckShaderCompileStatus()
{
	GLint compileResult;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		// Ha habido un error en la compilación.
		// Para saber qué ha pasado, tenemos que recuperar el mensaje de error de OpenGL
		GLint msgSize = 0;
		std::string message = "";
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &msgSize);
		if (msgSize > 0)
		{
			GLchar* message_c_str = new GLchar[msgSize];
			GLint writtenData = 0;
			glGetShaderInfoLog(shaderID, msgSize, &writtenData, message_c_str);
			message.assign(message_c_str);
			delete[] message_c_str;
			message_c_str = nullptr;

			throw std::runtime_error("ShaderObject::CheckShaderCompileStatus() -> Error al compilar Shader " + fileName + ": " + message);
		}
	}
}
