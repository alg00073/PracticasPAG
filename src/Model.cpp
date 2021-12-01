#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Model.h"

#include <iostream>

PAG::Model::Model(std::vector<Vertex> v, std::vector<GLuint> i) : vertex(v), index(i), shaderProgram(nullptr)
{
	GenerateVAO();
	GenerateVBO();
	GenerateIBO();
}

PAG::Model::Model(const char* path): shaderProgram(nullptr)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals);

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

	delete shaderProgram;
	delete material;

	shaderProgram, material = nullptr;
}

PAG::Model::Model(const Model& other): vertex(other.vertex), index(other.index)
{
	shaderProgram = new ShaderProgram(*other.shaderProgram);
	material = new Material(*other.material);
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

int PAG::Model::GetNumIndex()
{
	return index.size();
}

void PAG::Model::SetMaterial(Material* material)
{
	this->material = material;
}

PAG::Material* PAG::Model::GetMaterial()
{
	return material;
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

			vertex.push_back(Vertex(position, normal, texCoord));
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

	// Atributo 1
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0);

	// Atributo 2
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLubyte*)0 + offsetof(Vertex, normal));
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex.size(), vertex.data(), GL_STATIC_DRAW);
}

void PAG::Model::GenerateIBO()
{
	// IBO
	glGenBuffers(1, &idIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index.size(), index.data(), GL_STATIC_DRAW);
}
