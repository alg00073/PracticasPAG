#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Model.h"

PAG::Model::Model(std::vector<Vertex> v, std::vector<GLuint> i) : vertex(v), index(i), shaderProgram(nullptr)
{
	// VAO
	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);

	/* VBO entrelazado */
	glGenBuffers(1, &idVBO);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO);

	// Atributo 1
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0);

	// Atributo 2
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0 + offsetof(Vertex, color));
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex.size(), vertex.data(), GL_STATIC_DRAW);

	// IBO
	glGenBuffers(1, &idIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index.size(), index.data(), GL_STATIC_DRAW);
}

PAG::Model::~Model()
{
	vertex.clear();
	index.clear();

	glDeleteBuffers(1, &idVBO);
	glDeleteBuffers(1, &idIBO);
	glDeleteVertexArrays(1, &idVAO);

	delete shaderProgram;
}

PAG::Model::Model(const Model& other): vertex(other.vertex), index(other.index)
{
	shaderProgram = new ShaderProgram(*other.shaderProgram);
}

void PAG::Model::AssignShaderProgram(std::string vertexShader, std::string fragmentShader)
{
	try {
		shaderProgram = new ShaderProgram(vertexShader, fragmentShader);

		idSP = shaderProgram->GetID();
	}
	catch (std::exception& ex) {
		throw std::runtime_error("Model::AssignShaderProgram() -> " + (std::string)ex.what());
	}
}

GLuint PAG::Model::GetIdSP()
{
	return idSP;
}

GLuint PAG::Model::GetIdVAO()
{
	return idVAO;
}

GLuint PAG::Model::GetIdVBO()
{
	return idVBO;
}

GLuint PAG::Model::GetIdIBO()
{
	return idIBO;
}
