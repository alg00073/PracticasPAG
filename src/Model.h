#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace PAG {

	class Vertex {
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;

		Vertex(glm::vec3 p, glm::vec3 n) {
			position = p;
			normal = n;
			texCoord = glm::vec2(0, 0);
		}

		Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 tc) {
			position = p;
			normal = n;
			texCoord = tc;
		}
	};

	enum class ModelType
	{
		TRIANGLE,
		TETRAHEDRON,
		OBJ
	};

	class Model
	{
	private:

		ModelType modelType;

		std::vector<Vertex> vertex;
		std::vector<GLuint> index;

		ShaderProgram* shaderProgram;
		Material* material;

		GLuint idSP = 0; // Identificador del shader program
		GLuint idVAO = 0; // Identificador del vertex array object
		GLuint idVBO = 0; // Identificador del vertex buffer object
		GLuint idIBO = 0; // Identificador del index buffer object

		void processNode(aiNode* node, const aiScene* scene);

		void GenerateVAO();
		void GenerateVBO();
		void GenerateIBO();

	public:

		Model(ModelType type, Material* material = nullptr);
		Model(const char* path, Material* material = nullptr);

		Model(const Model& other);
		virtual ~Model();

		void AssignShaderProgram(std::string vertexShader, std::string fragmentShader);

		GLuint GetIdSP();
		GLuint GetIdVAO();
		GLuint GetIdVBO();
		GLuint GetIdIBO();

		int GetNumIndex();

		void SetMaterial(Material* material);
		Material* GetMaterial();
	};
}

