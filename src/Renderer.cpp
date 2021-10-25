#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Renderer.h"

PAG::Renderer* PAG::Renderer::instance = nullptr;

PAG::Renderer::Renderer()
{
	triangleModel = CreateTriangle();

	glm::vec3 cameraPosition(0, 0, -1);
	glm::vec3 cameraLookAt(0, 0, 4);
	float cameraFovX = glm::radians(90);
	float cameraNearZ = 0;
	float cameraFarZ = 3;
	int cameraHeight = 500;
	int cameraWidth = 500;

	virtualCamera = new Camera(cameraPosition, cameraLookAt, cameraFovX, cameraNearZ, cameraFarZ, cameraHeight, cameraWidth);
}

PAG::Renderer::~Renderer()
{
	delete triangleModel;
}

/**
 * M�todo para inicializar los par�metros globales de OpenGL
 */
void PAG::Renderer::InitializeOpenGL()
{
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	EnableDepth();
	glEnable(GL_MULTISAMPLE);
}

double* PAG::Renderer::GetClearColor()
{
	return clearColor;
}

void PAG::Renderer::SetClearColor(double* color)
{
	clearColor[0] = color[0];
	clearColor[1] = color[1];
	clearColor[2] = color[2];
	clearColor[3] = color[3];
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
}


PAG::Renderer* PAG::Renderer::Instance()
{
	if (instance == nullptr) { // Lazy initialization: si a�n no existe, lo crea
		instance = new Renderer();
	}
	return instance;
}

void PAG::Renderer::Refresh()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (activeModel) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(triangleModel->GetIdSP());
		glBindVertexArray(triangleModel->GetIdVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleModel->GetIdIBO());
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
	}
}

void PAG::Renderer::Resize(int width, int height)
{
	glViewport(0, 0, width, height);
}

void PAG::Renderer::EnableDepth()
{
	glEnable(GL_DEPTH_TEST);
}

void PAG::Renderer::ShoutInfo()
{
	std::cout << glGetString(GL_RENDERER) << std::endl
		<< glGetString(GL_VENDOR) << std::endl
		<< glGetString(GL_VERSION) << std::endl
		<< glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void PAG::Renderer::AddModel()
{
	if (!activeModel) {
		triangleModel = CreateTriangle();
	}

	activeModel = true;
}

void PAG::Renderer::DeleteModel()
{
	if (activeModel) {
		delete triangleModel;
	}

	activeModel = false;
}

PAG::Model* PAG::Renderer::CreateTriangle()
{
	glm::vec3 v1 = { -.5, -.5, 0.0 };
	glm::vec3 v2 = { .5, -.5, 0 };
	glm::vec3 v3 = { .0, .5, 0 };

	glm::vec3 c1 = { 1.0, 0.0, 0.0 };
	glm::vec3 c2 = { 0.0, 1.0, 0.0 };
	glm::vec3 c3 = { 0.0, 0.0, 1.0 };

	std::vector<Vertex> vertex =
	{
		Vertex(v1, c1),
		Vertex(v2, c2),
		Vertex(v3, c3)
	};

	std::vector<GLuint> index = { 0, 1, 2 };

	Model* triangle = new Model(vertex, index);
	triangle->AssignShaderProgram("vs", "fs");

	return triangle;
}
