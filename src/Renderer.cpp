#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Renderer.h"

PAG::Renderer* PAG::Renderer::instance = nullptr;

PAG::Renderer::Renderer()
{
	model = CreateTriangle();

#pragma region Camera Parameters

	glm::vec3 cameraPosition(0, 0, 2);
	glm::vec3 cameraLookAt(0, 0, 0);
	float cameraFovX = glm::radians(60.0f);
	float cameraNearZ = 0.5;
	float cameraFarZ = 6;
	int cameraHeight = 576;
	int cameraWidth = 1024;

#pragma endregion

	virtualCamera = new Camera(cameraPosition, cameraLookAt, cameraFovX,
		cameraNearZ, cameraFarZ, cameraHeight, cameraWidth);

#pragma region Lights Parameters

	glm::vec3 pointLightPosition(3, 3, 3);
	glm::vec3 spotLightPosition(0.25, 0.25, -1);
	glm::vec3 directionalLightDirection(0, 0, -1);

	glm::vec3 spotLightDirection(0, 0, 1);
	float spotlightAngle = 30;

	glm::vec3 ambientIntensity(0.4, 0.4, 0.4);
	glm::vec3 diffuseIntensity(0, 1, 0);
	glm::vec3 specularIntensity(0, 0, 1);

#pragma endregion

	sceneLights.push_back(new PointLight(pointLightPosition, diffuseIntensity, specularIntensity));
	sceneLights.push_back(new AmbientLight(ambientIntensity));
	//sceneLights.push_back(new DirectionalLight(directionalLightDirection, diffuseIntensity, specularIntensity));
	//sceneLights.push_back(new SpotLight(spotLightPosition, spotLightDirection, diffuseIntensity, specularIntensity, spotlightAngle));
}

PAG::Renderer::~Renderer()
{
	delete model;
	delete virtualCamera;
	delete sceneLights.data();

	sceneLights.clear();

	model, virtualCamera = nullptr;
}

/**
 * M�todo para inicializar los par�metros globales de OpenGL
 */
void PAG::Renderer::InitializeOpenGL()
{
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
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

		for (int i = 0; i < sceneLights.size(); i++) {

			glBlendFunc(GL_SRC_ALPHA, i == 0 ? GL_ONE_MINUS_SRC_ALPHA : GL_ONE);

			glUseProgram(model->GetIdSP());
			glBindVertexArray(model->GetIdVAO());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->GetIdIBO());
			glPolygonMode(GL_FRONT_AND_BACK, activeRenderMode == RenderMode::SOLID ? GL_FILL : GL_LINE);

#pragma region Common Uniforms

			// Uniform matriz modelado, vision y proyección
			std::string mModelViewProjName = "mModelViewProj";
			glm::mat4 mModelViewProj = virtualCamera->GetModelViewProjMatrix();
			SetUniform4fm(mModelViewProjName, mModelViewProj);

			// Uniform matriz modelado y vision
			std::string mModelViewName = "mModelView";
			glm::mat4 mModelView = virtualCamera->GetModelViewMatrix();
			SetUniform4fm(mModelViewName, mModelView);

			// Uniform color ambiente del material
			std::string KaName = "Ka";
			glm::vec3 Ka = model->GetMaterial()->GetAmbientColor();
			SetUniform3fv(KaName, Ka);

			// Uniform color difuso del material
			std::string KdName = "Kd";
			glm::vec3 Kd = model->GetMaterial()->GetDiffuseColor();
			SetUniform3fv(KdName, Kd);

			// Uniform color especular del material
			std::string KsName = "Ks";
			glm::vec3 Ks = model->GetMaterial()->GetSpecularColor();
			SetUniform3fv(KsName, Ks);

			// Uniform para pasar el phongExponent del material
			std::string phongExponentName = "phongExponent";
			float phongExponent = model->GetMaterial()->GetPhongExponent();
			SetUniform1f(phongExponentName, phongExponent);

#pragma endregion

#pragma region Light Uniforms

			std::string lightSubroutine;

			switch (sceneLights[i]->GetLightType())
			{
			case PAG::LightType::AMBIENT: {

				std::string IaName = "Ia";
				glm::vec3 Ia = dynamic_cast<AmbientLight*>(sceneLights[i])->GetAmbient();
				SetUniform3fv(IaName, Ia);

				lightSubroutine = "ambient";

				break;
			}
			case PAG::LightType::DIRECTIONAL: {

				std::string lightDirectionName = "lightDirection";
				glm::vec3 lightDirection = dynamic_cast<DirectionalLight*>(sceneLights[i])->GetDirection();
				SetUniform3fv(lightDirectionName, lightDirection);

				std::string IdName = "Id";
				glm::vec3 Id = dynamic_cast<DirectionalLight*>(sceneLights[i])->GetDiffuse();

				std::cout << dynamic_cast<DirectionalLight*>(sceneLights[i])->GetDiffuse().r << ", "
					<< dynamic_cast<DirectionalLight*>(sceneLights[i])->GetDiffuse().g << ", "
					<< dynamic_cast<DirectionalLight*>(sceneLights[i])->GetDiffuse().b << ", " << std::endl;

				SetUniform3fv(IdName, Id);

				std::string IsName = "Is";
				glm::vec3 Is = dynamic_cast<DirectionalLight*>(sceneLights[i])->GetSpecular();
				SetUniform3fv(IsName, Is);

				lightSubroutine = "directional";

				break;
			}
			case PAG::LightType::POINT: {

				std::string lightPositionName = "lightPosition";
				glm::vec3 lightPosition = dynamic_cast<PointLight*>(sceneLights[i])->GetPosition();
				SetUniform3fv(lightPositionName, lightPosition);

				std::string IdName = "Id";
				glm::vec3 Id = dynamic_cast<PointLight*>(sceneLights[i])->GetDiffuse();
				SetUniform3fv(IdName, Id);

				std::string IsName = "Is";
				glm::vec3 Is = dynamic_cast<PointLight*>(sceneLights[i])->GetSpecular();
				SetUniform3fv(IsName, Is);

				lightSubroutine = "point";

				break;
			}
			case PAG::LightType::SPOT: {

				std::string lightPositionName = "lightPosition";
				glm::vec3 lightPosition = dynamic_cast<SpotLight*>(sceneLights[i])->GetPosition();
				SetUniform3fv(lightPositionName, lightPosition);

				std::string lightDirectionName = "lightDirection";
				glm::vec3 lightDirection = dynamic_cast<SpotLight*>(sceneLights[i])->GetDirection();
				SetUniform3fv(lightDirectionName, lightDirection);

				std::string spotLightAngleName = "spotlightAngle";
				float spotLightAngle = dynamic_cast<SpotLight*>(sceneLights[i])->GetSpotlightAngle();
				SetUniform1f(spotLightAngleName, spotLightAngle);

				std::string IdName = "Id";
				glm::vec3 Id = dynamic_cast<SpotLight*>(sceneLights[i])->GetDiffuse();
				SetUniform3fv(IdName, Id);

				std::string IsName = "Is";
				glm::vec3 Is = dynamic_cast<SpotLight*>(sceneLights[i])->GetSpecular();
				SetUniform3fv(IsName, Is);

				lightSubroutine = "spot";

				break;
			}
			}

#pragma endregion

#pragma region Subroutines

			GLuint subroutineLocation = glGetSubroutineIndex(model->GetIdSP(), GL_FRAGMENT_SHADER, lightSubroutine.c_str());

			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &subroutineLocation);

#pragma endregion
			
			glDrawElements(GL_TRIANGLES, model->GetNumIndex(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void PAG::Renderer::Resize(int width, int height)
{
	virtualCamera->SetHeight(height);
	virtualCamera->SetWidth(width);

	glViewport(0, 0, width, height);
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

void PAG::Renderer::SetUniform1f(std::string name, float data)
{
	GLint location = glGetUniformLocation(model->GetIdSP(), name.c_str());
	if (location != -1) {
		glUniform1f(location, data);
	}
	else {
		std::cout << "SetUniform1f(): Cannot find localization for: " << name << std::endl;
	}
}

void PAG::Renderer::SetUniform3fv(std::string name, glm::vec3 data)
{
	GLint location = glGetUniformLocation(model->GetIdSP(), name.c_str());
	if (location != -1) {
		glUniform3fv(location, 1, &data[0]);
	}
	else {
		std::cout << "SetUniform3fv(): Cannot find localization for: " << name << std::endl;
	}
}

void PAG::Renderer::SetUniform4fm(std::string name, glm::mat4 data)
{
	GLint location = glGetUniformLocation(model->GetIdSP(), name.c_str());
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &data[0][0]);
	}
	else {
		std::cout << "SetUniform4fm(): Cannot find localization for: " << name << std::endl;
	}
}

PAG::Model* PAG::Renderer::CreateTriangle()
{
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

	glm::vec3 Ia(0.8, 0.3, 0.3);
	glm::vec3 Id(0.67, 0.16, 0.24);
	glm::vec3 Is(0.9, 0.9, 0.9);
	float Ns = 8.0;

	Model* triangle = new Model(vertex, index);
	Material* material = new Material(Ia, Id, Is, Ns);

	triangle->SetMaterial(material);
	triangle->AssignShaderProgram("vs", "fs");

	return triangle;
}

PAG::Model* PAG::Renderer::CreateTetrahedron()
{
	glm::vec3 v1 = { 0.7, 0.0, 0.0 };
	glm::vec3 v2 = { 0.0, 0.7, 0.0 };
	glm::vec3 v3 = { 0.0, 0.0, 0.7 };
	glm::vec3 v4 = { 0.0, 0.0, 0.0 };

	glm::vec3 n1 = { 1.0, 0.0, 0.0 };
	glm::vec3 n2 = { 0.0, 1.0, 0.0 };
	glm::vec3 n3 = { 0.0, 0.0, 1.0 };
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

	glm::vec3 Ia(0.9, 0.2, 0.2);
	glm::vec3 Id(0.67, 0.16, 0.24);
	glm::vec3 Is(0.9, 0.9, 0.9);
	float Ns = 128.0;

	Model* tetrahedron = new Model(vertex, index);
	Material* material = new Material(Ia, Id, Is, Ns);

	tetrahedron->SetMaterial(material);
	tetrahedron->AssignShaderProgram("vs", "fs");

	return tetrahedron;
}
