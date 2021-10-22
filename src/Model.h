#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "ShaderProgram.h"

namespace PAG {

	class Vertex {
	public:
		glm::vec3 position;
		glm::vec3 color;

		Vertex(glm::vec3 p, glm::vec3 c) {
			position = p;
			color = c;
		}
	};

	class Model
	{
	private:

		std::vector<Vertex> vertex;
		std::vector<GLuint> index;

		ShaderProgram* shaderProgram;

		GLuint idSP = 0; // Identificador del shader program
		GLuint idVAO = 0; // Identificador del vertex array object
		GLuint idVBO = 0; // Identificador del vertex buffer object
		GLuint idIBO = 0; // Identificador del index buffer object

	public:

		Model(std::vector<Vertex> v, std::vector<GLuint> i);
		Model(const Model& other);
		virtual ~Model();

		void AssignShaderProgram(std::string vertexShader, std::string fragmentShader);

		GLuint GetIdSP();
		GLuint GetIdVAO();
		GLuint GetIdVBO();
		GLuint GetIdIBO();
	};
}

