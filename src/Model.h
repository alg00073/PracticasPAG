#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "Material.h"
#include "Transform.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace PAG {

	class Vertex {
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec3 tangent;
		glm::vec3 bitangent;

		Vertex(glm::vec3 p, glm::vec3 n) {
			position = p;
			normal = n;
			texCoord = glm::vec2(0, 0);
			tangent = glm::vec3(0, 0, 0);
			bitangent = glm::vec3(0, 0, 0);
		}

		Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 tc) {
			position = p;
			normal = n;
			texCoord = tc;
			tangent = glm::vec3(0, 0, 0);
			bitangent = glm::vec3(0, 0, 0);
		}

		Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 tc, glm::vec3 t, glm::vec3 bt) {
			position = p;
			normal = n;
			texCoord = tc;
			tangent = t;
			bitangent = bt;
		}
	};

	enum class ModelType
	{
		TRIANGLE,
		TETRAHEDRON,
		OBJ
	};

	enum class RenderMode {
		SOLID,
		WIREFRAME,
		TEXTURE
	};

	class Model
	{
	private:

		ModelType modelType;
		RenderMode renderMode = RenderMode::SOLID;

		std::vector<Vertex> vertex;
		std::vector<GLuint> index;

		Transform* transform;
		
		Material* material;

		GLuint idVAO = 0; // Identificador del vertex array object
		GLuint idVBO = 0; // Identificador del vertex buffer object
		GLuint idIBO = 0; // Identificador del index buffer object

		void processNode(aiNode* node, const aiScene* scene);

		void GenerateVAO();
		void GenerateVBO();
		void GenerateIBO();

	public:

		Model(ModelType type, Material* material);
		Model(const char* path, Material* material);

		Model(const Model& other);
		virtual ~Model();

		GLuint GetIdVAO();
		GLuint GetIdVBO();
		GLuint GetIdIBO();

		int GetNumIndex();

		void SetRenderMode(RenderMode mode);
		RenderMode GetRenderMode();

		void SetMaterial(Material* material);
		Material* GetMaterial();

		Transform* GetTransform();
	};
}

