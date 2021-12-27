#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Model.h"

#include <iostream>

PAG::Model::Model(ModelType type, Material* material) : modelType(type), material(material), transform(new Transform())
{
	switch (type) {
	case ModelType::TRIANGLE: {

		glm::vec3 v1 = { -.5, -.5, 0.0 };
		glm::vec3 v2 = { .5, -.5, 0.0 };
		glm::vec3 v3 = { .0, .5, 0.0 };

		glm::vec3 n1 = { 0.0, 0.0, 1.0 };

		std::vector<Vertex> vertex =
		{
			Vertex(v1, n1),
			Vertex(v2, n1),
			Vertex(v3, n1)
		};

		std::vector<GLuint> index = { 0, 1, 2 };

		this->vertex = vertex;
		this->index = index;

		break;
	}
	case ModelType::TETRAHEDRON: {

		glm::vec3 v1 = { 0.7, 0.0, 0.0 };
		glm::vec3 v2 = { 0.0, 0.7, 0.0 };
		glm::vec3 v3 = { 0.0, 0.0, 0.7 };
		glm::vec3 v4 = { 0.0, 0.0, 0.0 };

		glm::vec3 n1 = { -1.0, 0.0, 0.0 };
		glm::vec3 n2 = { 0.0, -1.0, 0.0 };
		glm::vec3 n3 = { 0.0, 0.0, -1.0 };
		glm::vec3 n4 = { 1.0, 1.0, 1.0 };
		n4 = glm::normalize(n4);

		std::vector<Vertex> vertex =
		{
			// Triángulo 1 (0, 1, 2)
			Vertex(v1, n4),
			Vertex(v2, n4),
			Vertex(v3, n4),
			// Triángulo 2 (1, 0, 3)
			Vertex(v2, n3),
			Vertex(v1, n3),
			Vertex(v4, n3),
			// Triángulo 3 (2, 1, 3)
			Vertex(v3, n1),
			Vertex(v2, n1),
			Vertex(v4, n1),
			// Triángulo 4 (3, 0, 2)
			Vertex(v4, n2),
			Vertex(v1, n2),
			Vertex(v3, n2),
		};

		std::vector<GLuint> index = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

		this->vertex = vertex;
		this->index = index;

		break;
	}
	}

	GenerateVAO();
	GenerateVBO();
	GenerateIBO();
}

PAG::Model::Model(const char* path, Material* material) : modelType(ModelType::OBJ), material(material), transform(new Transform())
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace );

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error("Model() -> Assimp: " + (std::string)importer.GetErrorString());
	}

	processNode(scene->mRootNode, scene);

	GenerateVAO();
	GenerateVBO();
	GenerateIBO();
}

PAG::Model::~Model()
{
	vertex.clear();
	index.clear();

	glDeleteBuffers(1, &idVBO);
	glDeleteBuffers(1, &idIBO);
	glDeleteVertexArrays(1, &idVAO);

	delete transform;
	delete material;

	transform = nullptr;
	material = nullptr;
}

PAG::Model::Model(const Model& other) : vertex(other.vertex), index(other.index), modelType(other.modelType)
{
	transform = new Transform();
	material = new Material(*other.material);
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

int PAG::Model::GetNumIndex()
{
	return index.size();
}

void PAG::Model::SetRenderMode(RenderMode mode)
{
	if (mode == RenderMode::TEXTURE && material->GetTexture() == nullptr) {
		renderMode = RenderMode::SOLID;
		throw std::runtime_error("SetRenderMode() -> El modelo no tiene textura asociada");
	}
	else {
		renderMode = mode;
	}
}

PAG::RenderMode PAG::Model::GetRenderMode()
{
	return renderMode;
}

void PAG::Model::SetMaterial(Material* material)
{
	this->material = material;
}

PAG::Material* PAG::Model::GetMaterial()
{
	return material;
}

PAG::Transform* PAG::Model::GetTransform()
{
	return transform;
}

void PAG::Model::RenderModel()
{
	glBindVertexArray(idVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, nullptr);
}

void PAG::Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			// process vertex positions, normals and texture coordinates
			glm::vec3 position;
			position.x = mesh->mVertices[i].x;
			position.y = mesh->mVertices[i].y;
			position.z = mesh->mVertices[i].z;

			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;

			glm::vec2 texCoord(0, 0);

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				texCoord.x = mesh->mTextureCoords[0][i].x;
				texCoord.y = mesh->mTextureCoords[0][i].y;
			}

			glm::vec3 tangent(0.0f, 0.0f, 0.0f);
			glm::vec3 bitangent(0.0f, 0.0f, 0.0f);

			if (mesh->mTangents) {

				tangent.x = mesh->mTangents[i].x;
				tangent.y = mesh->mTangents[i].y;
				tangent.z = mesh->mTangents[i].z;

				bitangent.x = mesh->mBitangents[i].x;
				bitangent.y = mesh->mBitangents[i].y;
				bitangent.z = mesh->mBitangents[i].z;
			}

			vertex.push_back(Vertex(position, normal, texCoord, tangent, bitangent));
		}

		// process index
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				index.push_back(face.mIndices[j]);
		}
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void PAG::Model::GenerateVAO()
{
	// VAO
	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);
}

void PAG::Model::GenerateVBO()
{
	/* VBO entrelazado */
	glGenBuffers(1, &idVBO);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO);

	// Atributo 1: Posición
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0);

	// Atributo 2: Normales
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0 + offsetof(Vertex, normal));

	// Atributo 3: Coordenadas de textura
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0 + offsetof(Vertex, texCoord));

	// Atributo 4: Tangentes
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0 + offsetof(Vertex, tangent));

	// Atributo 5: Bitangentes
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0 + offsetof(Vertex, bitangent));

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex.size(), vertex.data(), GL_STATIC_DRAW);
}

void PAG::Model::GenerateIBO()
{
	// IBO
	glGenBuffers(1, &idIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index.size(), index.data(), GL_STATIC_DRAW);
}
