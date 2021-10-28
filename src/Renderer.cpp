#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Renderer.h"

PAG::Renderer* PAG::Renderer::instance = nullptr;

PAG::Renderer::Renderer()
{
	model = CreateTriangle();

	glm::vec3 cameraPosition(0, 0, 2);
	glm::vec3 cameraLookAt(0, 0, 0);
	float cameraFovX = glm::radians(60.0f);
	float cameraNearZ = 0.5;
	float cameraFarZ = 6;
	int cameraHeight = 576;
	int cameraWidth = 1024;

	virtualCamera = new Camera(cameraPosition, cameraLookAt, cameraFovX, cameraNearZ, cameraFarZ, cameraHeight, cameraWidth);
}

PAG::Renderer::~Renderer()
{
	delete model;
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

	if (activeModel != -1) {

		// - Pasarle los uniforms —————————————
		std::string mModelViewProjName = "mModelViewProj";
		glm::mat4 mModelViewProj = virtualCamera->GetModelViewProjMatrix();

		GLint location = glGetUniformLocation(model->GetIdSP(), mModelViewProjName.c_str());
		if (location != -1) {
			glUniformMatrix4fv(location, 1, GL_FALSE, &mModelViewProj[0][0]);
		}
		else {
			std::cout << "Cannot find localization for: " << mModelViewProjName << std::endl;
		}

		glUseProgram(model->GetIdSP());

		GLuint aux;

		switch (activeRenderMode) {
		case RenderMode::SOLID:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			aux = glGetSubroutineIndex(model->GetIdSP(), GL_FRAGMENT_SHADER,
				"colorSolid");

			break;
		case RenderMode::WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			aux = glGetSubroutineIndex(model->GetIdSP(), GL_FRAGMENT_SHADER,
				"colorWireframe");

			break;
		}

		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &aux);

		glBindVertexArray(model->GetIdVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->GetIdIBO());
		glDrawElements(GL_TRIANGLES, model->GetNumIndex(), GL_UNSIGNED_INT, nullptr);
	}
}

void PAG::Renderer::Resize(int width, int height)
{
	virtualCamera->SetHeight(height);
	virtualCamera->SetWidth(width);

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

void PAG::Renderer::SwitchModel()
{
	if (activeModel != -1)
	{
		delete model;
		activeModel = (activeModel + 1) % 2;
	}
	else
	{
		activeModel = 0;
	}

	switch (activeModel) {
	case 0:
		model = CreateTriangle();
		break;
	case 1:
		model = CreateTetrahedron();
		break;
	default:
		break;
	}

	Refresh();
}

void PAG::Renderer::DeleteModel()
{
	if (activeModel != -1) {
		delete model;
	}

	activeModel = -1;
}

void PAG::Renderer::ChangeCameraMovement(PAG::MovementType type)
{
	activeMovementType = type;
}

void PAG::Renderer::ApplyCameraMovement(double deltaX, double deltaY)
{
	try {
		virtualCamera->ApplyMovement(deltaX, deltaY, activeMovementType);
	}
	catch (std::exception& ex) {
		throw std::runtime_error("Renderer::ApplyCameraMovement() -> " + std::string(ex.what()));
	}
}

void PAG::Renderer::ResetCamera()
{
	virtualCamera->Reset();
}

void PAG::Renderer::ChangeRenderMode(PAG::RenderMode mode)
{
	activeRenderMode = mode;
}

PAG::Model* PAG::Renderer::CreateTriangle()
{
	glm::vec3 v1 = { -.5, -.5, 0.0 };
	glm::vec3 v2 = { .5, -.5, 0.0 };
	glm::vec3 v3 = { .0, .5, 0.0 };

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

PAG::Model* PAG::Renderer::CreateTetrahedron()
{
	glm::vec3 v1 = { 0.5, 0.0, 0.0 };
	glm::vec3 v2 = { 0.0, 0.5, 0.0 };
	glm::vec3 v3 = { 0.0, 0.0, 0.5 };
	glm::vec3 v4 = { 0.0, 0.0, 0.0 };

	glm::vec3 c1 = { 1.0, 0.0, 0.0 };
	glm::vec3 c2 = { 0.0, 1.0, 0.0 };
	glm::vec3 c3 = { 0.0, 0.0, 1.0 };
	glm::vec3 c4 = { 1.0, 0.0, 1.0 };

	std::vector<Vertex> vertex =
	{
		Vertex(v1, c1),
		Vertex(v2, c2),
		Vertex(v3, c3),
		Vertex(v4, c4)
	};

	// Triángulo 1 (0, 1, 2)
	// Triángulo 2 (1, 0, 3)
	// Triángulo 3 (2, 1, 3)
	// Triángulo 4 (3, 0, 2)
	std::vector<GLuint> index = { 0, 1, 2, 1, 0, 3, 2, 1, 3, 3, 0, 2 };

	Model* tetrahedron = new Model(vertex, index);
	tetrahedron->AssignShaderProgram("vs", "fs");

	return tetrahedron;
}
