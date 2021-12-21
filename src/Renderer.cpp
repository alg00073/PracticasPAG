#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Renderer.h"

PAG::Renderer* PAG::Renderer::instance = nullptr;

PAG::Renderer::Renderer()
{
	AddModel(0);

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

					ShaderProgram* shaderProgramToUse = models[i]->GetMaterial()->GetShaderProgram();

					glUseProgram(shaderProgramToUse->GetID());
					glBindVertexArray(models[i]->GetIdVAO());
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i]->GetIdIBO());

#pragma region Common Uniforms

					// Uniform matriz modelado, vision y proyección
					std::string mModelViewProjName = "mModelViewProj";
					glm::mat4 mModelViewProj = virtualCamera->GetViewProjMatrix() * models[i]->GetTransform()->GetModelMatrix();
					shaderProgramToUse->SetUniform4fm(mModelViewProjName, mModelViewProj);

					// Uniform matriz modelado y vision
					std::string mModelViewName = "mModelView";
					glm::mat4 mModelView = virtualCamera->GetViewMatrix() * models[i]->GetTransform()->GetModelMatrix();
					shaderProgramToUse->SetUniform4fm(mModelViewName, mModelView);

					// Uniform matriz modelado y vision (inversa de la traspuesta)
					std::string mModelViewITName = "mModelViewIT";
					shaderProgramToUse->SetUniform4fm(mModelViewITName, glm::inverse(glm::transpose(mModelView)));

					// Uniform color ambiente del material
					std::string KaName = "Ka";
					glm::vec3 Ka = models[i]->GetMaterial()->GetAmbientColor();
					shaderProgramToUse->SetUniform3fv(KaName, Ka);

					// Uniform color difuso del material
					std::string KdName = "Kd";
					glm::vec3 Kd = models[i]->GetMaterial()->GetDiffuseColor();
					shaderProgramToUse->SetUniform3fv(KdName, Kd);

					// Uniform color especular del material
					std::string KsName = "Ks";
					glm::vec3 Ks = models[i]->GetMaterial()->GetSpecularColor();
					shaderProgramToUse->SetUniform3fv(KsName, Ks);

					// Uniform para pasar el phongExponent del material
					std::string phongExponentName = "phongExponent";
					float phongExponent = models[i]->GetMaterial()->GetPhongExponent();
					shaderProgramToUse->SetUniform1f(phongExponentName, phongExponent);

#pragma endregion

#pragma region Light Uniforms

					glm::mat4 viewMatrix = virtualCamera->GetViewMatrix();

					std::string lightSubroutine;

					switch (sceneLights[j]->GetLightType())
					{
					case PAG::LightType::AMBIENT: {

						std::string IaName = "Ia";
						glm::vec3 Ia = dynamic_cast<AmbientLight*>(sceneLights[j])->GetAmbient();
						shaderProgramToUse->SetUniform3fv(IaName, Ia);

						lightSubroutine = "ambient";

						break;
					}
					case PAG::LightType::DIRECTIONAL: {

						std::string lightDirectionName = "lightDirection";
						glm::vec3 lightDirection = dynamic_cast<DirectionalLight*>(sceneLights[j])->GetDirection();
						glm::vec4 lightDirectionView = glm::transpose(glm::inverse(viewMatrix)) * glm::vec4(lightDirection, 0.0);

						shaderProgramToUse->SetUniform3fv(lightDirectionName, glm::vec3(lightDirectionView));

						std::string IdName = "Id";
						glm::vec3 Id = dynamic_cast<DirectionalLight*>(sceneLights[j])->GetDiffuse();

						shaderProgramToUse->SetUniform3fv(IdName, Id);

						std::string IsName = "Is";
						glm::vec3 Is = dynamic_cast<DirectionalLight*>(sceneLights[j])->GetSpecular();
						shaderProgramToUse->SetUniform3fv(IsName, Is);

						lightSubroutine = "directional";

						break;
					}
					case PAG::LightType::POINT: {

						std::string lightPositionName = "lightPosition";
						glm::vec3 lightPosition = dynamic_cast<PointLight*>(sceneLights[j])->GetPosition();
						glm::vec4 lightPositionView = viewMatrix * glm::vec4(lightPosition, 1.0);

						shaderProgramToUse->SetUniform3fv(lightPositionName, glm::vec3(lightPositionView));

						std::string IdName = "Id";
						glm::vec3 Id = dynamic_cast<PointLight*>(sceneLights[j])->GetDiffuse();
						shaderProgramToUse->SetUniform3fv(IdName, Id);

						std::string IsName = "Is";
						glm::vec3 Is = dynamic_cast<PointLight*>(sceneLights[j])->GetSpecular();
						shaderProgramToUse->SetUniform3fv(IsName, Is);

						lightSubroutine = "point";

						break;
					}
					case PAG::LightType::SPOT: {

						std::string lightPositionName = "lightPosition";
						glm::vec3 lightPosition = dynamic_cast<SpotLight*>(sceneLights[j])->GetPosition();
						glm::vec4 lightPositionView = viewMatrix * glm::vec4(lightPosition, 1.0);

						shaderProgramToUse->SetUniform3fv(lightPositionName, glm::vec3(lightPositionView));

						std::string lightDirectionName = "lightDirection";
						glm::vec3 lightDirection = dynamic_cast<SpotLight*>(sceneLights[j])->GetDirection();
						glm::vec4 lightDirectionView = glm::transpose(glm::inverse(viewMatrix)) * glm::vec4(lightDirection, 0.0);

						shaderProgramToUse->SetUniform3fv(lightDirectionName, glm::vec3(lightDirectionView));

						std::string spotLightAngleName = "spotlightAngle";
						float spotLightAngle = dynamic_cast<SpotLight*>(sceneLights[j])->GetSpotlightAngle();
						shaderProgramToUse->SetUniform1f(spotLightAngleName, spotLightAngle);

						std::string IdName = "Id";
						glm::vec3 Id = dynamic_cast<SpotLight*>(sceneLights[j])->GetDiffuse();
						shaderProgramToUse->SetUniform3fv(IdName, Id);

						std::string IsName = "Is";
						glm::vec3 Is = dynamic_cast<SpotLight*>(sceneLights[j])->GetSpecular();
						shaderProgramToUse->SetUniform3fv(IsName, Is);

						lightSubroutine = "spot";

						break;
					}
					}

#pragma endregion

					std::string renderSubroutine;

					switch (models[i]->GetRenderMode()) {
					case RenderMode::SOLID: {
						renderSubroutine = "materialColor";
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						break;
					}
					case RenderMode::TEXTURE: {
						renderSubroutine = "textureColor";
						shaderProgramToUse->SetUniformSampler2D("textureSampler", 0);
						models[i]->GetMaterial()->GetTexture()->ActivateTexture(0);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						break;
					}
					case RenderMode::WIREFRAME: {
						renderSubroutine = "wireframeColor";
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						break;
					}
					}

					shaderProgramToUse->SetSubroutineUniforms(lightSubroutine, renderSubroutine);

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

int PAG::Renderer::SwitchActiveModel()
{
	if (models.size() == 0) {
		activeModel = -1;
	}
	else {
		activeModel = (activeModel + 1) % models.size();
	}

	return activeModel;
}

void PAG::Renderer::AddModel(int model)
{
	glm::vec3 Ia(0.6, 0.2, 0.3);
	glm::vec3 Id(0.7, 0.2, 0.1);
	glm::vec3 Is(1, 1, 1);
	float Ns = 5.0;

	Material* material = new Material(Ia, Id, Is, Ns);
	material->SetShaderProgram("vs", "fs");

	switch (model) {
	case 0:
		models.push_back(new Model(PAG::ModelType::TRIANGLE, material));
		break;
	case 1:
		models.push_back(new Model(PAG::ModelType::TETRAHEDRON, material));
		break;
	case 2:
		try {
			material->SetTexture(new Texture(".\\textures\\dado.png"));
			models.push_back(new Model(".\\models\\dado.obj", material));
		}
		catch (std::exception& ex) {
			std::cout << "AddModel() -> " + (std::string)ex.what() << std::endl;
		}
		break;
	case 3:
		material->SetTexture(new Texture(".\\textures\\spot_texture.png"));
		models.push_back(new Model(".\\models\\vaca.obj", material));
		break;
	default:
		break;
	}

	if (models.size() == 1) {
		SwitchActiveModel();
	}

	Refresh();
}

void PAG::Renderer::DeleteModel()
{
	if (activeModel != -1) {
		models.erase(models.begin() + activeModel);
		SwitchActiveModel();
	}
}

std::string PAG::Renderer::SwitchTransformMode()
{
	if (activeModel != -1) {
		switch (activeTransformMode) {
		case TransformMode::TRANSLATE:
			activeTransformMode = TransformMode::ROTATE;
			return "ROTATE";
			break;
		case TransformMode::ROTATE:
			activeTransformMode = TransformMode::SCALE;
			return "SCALE";
			break;
		case TransformMode::SCALE:
			activeTransformMode = TransformMode::TRANSLATE;
			return "TRANSLATE";
			break;
		}
	}
}

void PAG::Renderer::ApplyTransform(glm::vec3 deltaTransform)
{
	if (activeModel != -1) {
		switch (activeTransformMode) {
		case TransformMode::TRANSLATE:
			models[activeModel]->GetTransform()->SetPosition(
				glm::vec3(
					models[activeModel]->GetTransform()->GetPosition().x + deltaTransform.x,
					models[activeModel]->GetTransform()->GetPosition().y + deltaTransform.y,
					models[activeModel]->GetTransform()->GetPosition().z + deltaTransform.z
				)
			);
			break;
		case TransformMode::ROTATE:
			models[activeModel]->GetTransform()->SetRotation(
				glm::vec3(
					models[activeModel]->GetTransform()->GetRotation().x + deltaTransform.x * 100,
					models[activeModel]->GetTransform()->GetRotation().y + deltaTransform.y * 100,
					models[activeModel]->GetTransform()->GetRotation().z + deltaTransform.z * 100
				)
			);
			break;
		case TransformMode::SCALE:
			models[activeModel]->GetTransform()->SetScale(
				glm::vec3(
					models[activeModel]->GetTransform()->GetScale().x + deltaTransform.x,
					models[activeModel]->GetTransform()->GetScale().y + deltaTransform.y,
					models[activeModel]->GetTransform()->GetScale().z + deltaTransform.z
				)
			);
			break;
		}
	}
}

void PAG::Renderer::ChangeCameraMovement(PAG::CameraMovementType type)
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

void PAG::Renderer::SwitchRenderMode()
{
	if (activeModel != -1) {
		switch (models[activeModel]->GetRenderMode()) {
		case RenderMode::SOLID:
			models[activeModel]->SetRenderMode(RenderMode::WIREFRAME);
			break;
		case RenderMode::WIREFRAME: 
			try {
				models[activeModel]->SetRenderMode(RenderMode::TEXTURE);
			}
			catch (std::exception& ex) {
				std::cout << ex.what() << std::endl;
			}
			break;
		case RenderMode::TEXTURE: 
			models[activeModel]->SetRenderMode(RenderMode::SOLID);
			break;
		}
	}
}

