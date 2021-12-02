#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Renderer.h"

PAG::Renderer* PAG::Renderer::instance = nullptr;

PAG::Renderer::Renderer()
{
	models.push_back(CreateTriangle());

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

	glm::vec3 pointLightPosition(-0.125, 0.275, 0.25);
	glm::vec3 spotLightPosition(0.7, 0.7, 0.7);
	glm::vec3 directionalLightDirection(0, 1, 1);

	glm::vec3 spotLightDirection(-1, -1, -1);
	float spotlightAngle = 15;

	glm::vec3 ambientIntensity(0.5, 0.5, 0.5);
	glm::vec3 diffuseIntensity(1, 1, 1);
	glm::vec3 specularIntensity(1, 1, 1);

#pragma endregion

	sceneLights.push_back(new PointLight(pointLightPosition, diffuseIntensity, specularIntensity));
	sceneLights.push_back(new AmbientLight(ambientIntensity));
	sceneLights.push_back(new DirectionalLight(glm::normalize(directionalLightDirection), diffuseIntensity, specularIntensity));
	sceneLights.push_back(new SpotLight(spotLightPosition, glm::normalize(spotLightDirection), diffuseIntensity, specularIntensity, spotlightAngle));
}

PAG::Renderer::~Renderer()
{
	delete[] models.data();
	delete[] sceneLights.data();
	delete virtualCamera;

	models.clear();
	sceneLights.clear();

	virtualCamera = nullptr;
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
	glDepthFunc(GL_LEQUAL);
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

	try {
		if (activeModel != -1) {

			for (int i = 0; i < models.size(); i++) {

				for (int j = 0; j < sceneLights.size(); j++) {

					glBlendFunc(GL_SRC_ALPHA, j == 0 ? GL_ONE_MINUS_SRC_ALPHA : GL_ONE);

					glUseProgram(models[i]->GetIdSP());
					glBindVertexArray(models[i]->GetIdVAO());
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i]->GetIdIBO());
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
					glm::vec3 Ka = models[i]->GetMaterial()->GetAmbientColor();
					SetUniform3fv(KaName, Ka);

					// Uniform color difuso del material
					std::string KdName = "Kd";
					glm::vec3 Kd = models[i]->GetMaterial()->GetDiffuseColor();
					SetUniform3fv(KdName, Kd);

					// Uniform color especular del material
					std::string KsName = "Ks";
					glm::vec3 Ks = models[i]->GetMaterial()->GetSpecularColor();
					SetUniform3fv(KsName, Ks);

					// Uniform para pasar el phongExponent del material
					std::string phongExponentName = "phongExponent";
					float phongExponent = models[i]->GetMaterial()->GetPhongExponent();
					SetUniform1f(phongExponentName, phongExponent);

#pragma endregion

#pragma region Light Uniforms

					std::string lightSubroutine;

					switch (sceneLights[j]->GetLightType())
					{
					case PAG::LightType::AMBIENT: {

						std::string IaName = "Ia";
						glm::vec3 Ia = dynamic_cast<AmbientLight*>(sceneLights[j])->GetAmbient();
						SetUniform3fv(IaName, Ia);

						lightSubroutine = "ambient";

						break;
					}
					case PAG::LightType::DIRECTIONAL: {

						std::string lightDirectionName = "lightDirection";
						glm::vec3 lightDirection = dynamic_cast<DirectionalLight*>(sceneLights[j])->GetDirection();
						glm::vec4 lightDirectionView = glm::transpose(glm::inverse(mModelView)) * glm::vec4(lightDirection, 0.0);

						SetUniform3fv(lightDirectionName, glm::vec3(lightDirectionView));

						std::string IdName = "Id";
						glm::vec3 Id = dynamic_cast<DirectionalLight*>(sceneLights[j])->GetDiffuse();

						SetUniform3fv(IdName, Id);

						std::string IsName = "Is";
						glm::vec3 Is = dynamic_cast<DirectionalLight*>(sceneLights[j])->GetSpecular();
						SetUniform3fv(IsName, Is);

						lightSubroutine = "directional";

						break;
					}
					case PAG::LightType::POINT: {

						std::string lightPositionName = "lightPosition";
						glm::vec3 lightPosition = dynamic_cast<PointLight*>(sceneLights[j])->GetPosition();
						glm::vec4 lightPositionView = mModelView * glm::vec4(lightPosition, 1.0);

						SetUniform3fv(lightPositionName, glm::vec3(lightPositionView));

						std::string IdName = "Id";
						glm::vec3 Id = dynamic_cast<PointLight*>(sceneLights[j])->GetDiffuse();
						SetUniform3fv(IdName, Id);

						std::string IsName = "Is";
						glm::vec3 Is = dynamic_cast<PointLight*>(sceneLights[j])->GetSpecular();
						SetUniform3fv(IsName, Is);

						lightSubroutine = "point";

						break;
					}
					case PAG::LightType::SPOT: {

						std::string lightPositionName = "lightPosition";
						glm::vec3 lightPosition = dynamic_cast<SpotLight*>(sceneLights[j])->GetPosition();
						glm::vec4 lightPositionView = mModelView * glm::vec4(lightPosition, 1.0);

						SetUniform3fv(lightPositionName, glm::vec3(lightPositionView));

						std::string lightDirectionName = "lightDirection";
						glm::vec3 lightDirection = dynamic_cast<SpotLight*>(sceneLights[j])->GetDirection();
						glm::vec4 lightDirectionView = glm::transpose(glm::inverse(mModelView)) * glm::vec4(lightDirection, 0.0);

						SetUniform3fv(lightDirectionName, glm::vec3(lightDirectionView));

						std::string spotLightAngleName = "spotlightAngle";
						float spotLightAngle = dynamic_cast<SpotLight*>(sceneLights[j])->GetSpotlightAngle();
						SetUniform1f(spotLightAngleName, spotLightAngle);

						std::string IdName = "Id";
						glm::vec3 Id = dynamic_cast<SpotLight*>(sceneLights[j])->GetDiffuse();
						SetUniform3fv(IdName, Id);

						std::string IsName = "Is";
						glm::vec3 Is = dynamic_cast<SpotLight*>(sceneLights[j])->GetSpecular();
						SetUniform3fv(IsName, Is);

						lightSubroutine = "spot";

						break;
					}
					}

#pragma endregion

					SetSubroutineUniform(lightSubroutine);

					glDrawElements(GL_TRIANGLES, models[i]->GetNumIndex(), GL_UNSIGNED_INT, nullptr);
				}
			}
		}
	}
	catch (std::runtime_error ex) {
		std::cout << ex.what() << std::endl;
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

int PAG::Renderer::SwitchModel()
{
	if (models.size() == 0) {
		activeModel = -1;
	}
	else {
		activeModel = (activeModel + 1) % models.size();
	}

	return activeModel;
}

void PAG::Renderer::AddModel()
{
	glm::vec3 Ia(0.6, 0.2, 0.3);
	glm::vec3 Id(0.7, 0.2, 0.1);
	glm::vec3 Is(1, 1, 1);
	float Ns = 5.0;

	Material* material = new Material(Ia, Id, Is, Ns);

	switch (models.size()) {
	case 0:
		models.push_back(new Model(PAG::ModelType::TRIANGLE, material));
		break;
	case 1:
		models.push_back(CreateTetrahedron());
		break;
	case 2:
		models.push_back(CreateModel(".\\models\\dado.obj"));
		break;
	case 3:
		models.push_back(CreateModel(".\\models\\vaca.obj"));
		break;
	default:
		break;
	}

	Refresh();
}

void PAG::Renderer::DeleteModel()
{
	if (activeModel != -1) {
		models.erase(models.begin() + activeModel);
		SwitchModel();
	}
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
	GLint location = glGetUniformLocation(models[activeModel]->GetIdSP(), name.c_str());
	if (location != -1) {
		glUniform1f(location, data);
	}
	else {
		throw std::runtime_error("SetUniform1f(): Cannot find localization for: " + name);
	}
}

void PAG::Renderer::SetUniform3fv(std::string name, glm::vec3 data)
{
	GLint location = glGetUniformLocation(models[activeModel]->GetIdSP(), name.c_str());
	if (location != -1) {
		glUniform3fv(location, 1, &data[0]);
	}
	else {
		throw std::runtime_error("SetUniform3fv(): Cannot find localization for: " + name);
	}
}

void PAG::Renderer::SetUniform4fm(std::string name, glm::mat4 data)
{
	GLint location = glGetUniformLocation(models[activeModel]->GetIdSP(), name.c_str());
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &data[0][0]);
	}
	else {
		throw std::runtime_error("SetUniform4fm(): Cannot find localization for: " + name);
	}
}

void PAG::Renderer::SetSubroutineUniform(std::string name)
{
	GLuint subroutineLocation = glGetSubroutineIndex(models[activeModel]->GetIdSP(), GL_FRAGMENT_SHADER, name.c_str());

	if (subroutineLocation != -1) {
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &subroutineLocation);
	}
	else {
		throw std::runtime_error("SetSubroutineUniform(): Cannot find localization for: " + subroutineLocation);
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

	glm::vec3 Ia(0.6, 0.2, 0.3);
	glm::vec3 Id(0.7, 0.2, 0.1);
	glm::vec3 Is(1, 1, 1);
	float Ns = 5.0;

	Material* material = new Material(Ia, Id, Is, Ns);
	Model* tetrahedron = new Model(vertex, index);

	tetrahedron->SetMaterial(material);
	tetrahedron->AssignShaderProgram("vs", "fs");

	return tetrahedron;
}

PAG::Model* PAG::Renderer::CreateModel(std::string path)
{
	Model* model = nullptr;

	try {
		model = new Model(path.c_str());
	}
	catch (std::runtime_error ex) {
		std::cout << ex.what() << std::endl;
	}

	glm::vec3 Ia(0.6, 0.2, 0.3);
	glm::vec3 Id(0.7, 0.2, 0.1);
	glm::vec3 Is(1, 1, 1);
	float Ns = 5.0;

	Material* material = new Material(Ia, Id, Is, Ns);

	if (model != nullptr) {
		model->SetMaterial(material);
		model->AssignShaderProgram("vs", "fs");
	}

	return model;
}
