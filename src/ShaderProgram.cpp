#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "ShaderProgram.h"

#include <vector>

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

void PAG::ShaderProgram::SetUniform1f(std::string name, float data)
{
	GLint location = glGetUniformLocation(idSP, name.c_str());
	if (location != -1) {
		glUniform1f(location, data);
	}
	else {
		throw std::runtime_error("SetUniform1f(): Cannot find localization for: " + name);
	}
}

void PAG::ShaderProgram::SetUniform3fv(std::string name, glm::vec3 data)
{
	GLint location = glGetUniformLocation(idSP, name.c_str());
	if (location != -1) {
		glUniform3fv(location, 1, &data[0]);
	}
	else {
		throw std::runtime_error("SetUniform3fv(): Cannot find localization for: " + name);
	}
}

void PAG::ShaderProgram::SetUniform4fm(std::string name, glm::mat4 data)
{
	GLint location = glGetUniformLocation(idSP, name.c_str());
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &data[0][0]);
	}
	else {
		throw std::runtime_error("SetUniform4fm(): Cannot find localization for: " + name);
	}
}

void PAG::ShaderProgram::SetUniformSampler2D(std::string name, GLint sampler)
{
	GLint posicion = glGetUniformLocation(idSP, name.c_str());
	glUniform1i(posicion, sampler);
}

void PAG::ShaderProgram::SetSubroutineUniforms(std::string lightName, std::string renderName)
{
	GLuint subroutines[2];

	GLint renderLocation = glGetSubroutineUniformLocation(idSP, GL_FRAGMENT_SHADER, "colorType");
	GLint lightLocation = glGetSubroutineUniformLocation(idSP, GL_FRAGMENT_SHADER, "lightType");

	if (renderLocation == -1 || lightLocation == -1)
	{
		throw std::runtime_error("SetSubroutineUniforms() -> No se ha podido localizar el uniform :" + renderName);
	}

	GLuint renderIndex = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, renderName.c_str());
	GLuint lightIndex = glGetSubroutineIndex(idSP, GL_FRAGMENT_SHADER, lightName.c_str());

	if (renderIndex != -1 && lightIndex != -1) {

		subroutines[renderLocation] = renderIndex;
		subroutines[lightLocation] = lightIndex;

		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
	}
	else {
		throw std::runtime_error("SetSubroutineUniforms(): Cannot find localization for subroutines");
	}
}

