#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

#include "Renderer.h"
#include <glm/gtx/transform.hpp>

PAG::Renderer* PAG::Renderer::instance = nullptr;

PAG::Renderer::Renderer()
{
	shaderProgramShadowMap = new ShaderProgram("vs-shadow", "fs-shadow");

#pragma region Setting Scene

	AddModel(3);
	AddModel(2);

	models[0]->GetTransform()->SetPosition(glm::vec3(0, -.65, 0));
	models[0]->SetRenderMode(RenderMode::TEXTURE);

	models[1]->GetTransform()->SetPosition(glm::vec3(0, .65, 0));
	models[1]->SetRenderMode(RenderMode::TEXTURE);

#pragma endregion

#pragma region Camera Parameters

	glm::vec3 cameraPosition(0, 0, 2);
	glm::vec3 cameraLookAt(0, 0, 0);
	float cameraFovX = glm::radians(90.0f);
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
	glm::vec3 directionalLightDirection(0, 1, 0);

	glm::vec3 spotLightDirection(-1, -1, -1);
	float spotlightAngle = 15;

	glm::vec3 ambientIntensity(0.5, 0.5, 0.5);
	glm::vec3 diffuseIntensity(1, 1, 1);
	glm::vec3 specularIntensity(1, 1, 1);

#pragma endregion

	//sceneLights.push_back(new Light(pointLightPosition, diffuseIntensity, specularIntensity, LightType::POINT));
	sceneLights.push_back(new Light(ambientIntensity));
	sceneLights.push_back(new Light(glm::normalize(directionalLightDirection), diffuseIntensity, specularIntensity, LightType::DIRECTIONAL));
	//sceneLights.push_back(new Light(spotLightPosition, glm::normalize(spotLightDirection), diffuseIntensity, specularIntensity, spotlightAngle));
}

PAG::Renderer::~Renderer()
{
	delete[] models.data();
	delete[] sceneLights.data();
	delete virtualCamera;
	delete shaderProgramShadowMap;

	models.clear();
	sceneLights.clear();

	virtualCamera = nullptr;
	shaderProgramShadowMap = nullptr;
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

	InitializeShadowMapping();
}

void PAG::Renderer::InitializeShadowMapping()
{
	shadowFBO = 0;
	glGenFramebuffers(1, &shadowFBO);
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
	if (needRecalculateShadowMap) {
		needRecalculateShadowMap = false;
		RecalculateShadowMaps();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, virtualCamera->GetWidth(), virtualCamera->GetHeight());

	try {
		if (activeModel != -1) {

			for (int j = 0; j < sceneLights.size(); j++) {
				for (int i = 0; i < models.size(); i++) {

					glBlendFunc(GL_SRC_ALPHA, j == 0 ? GL_ONE_MINUS_SRC_ALPHA : GL_ONE);
					glBindVertexArray(models[i]->GetIdVAO());
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i]->GetIdIBO());

					std::string renderSubroutine;
					std::string lightSubroutine;

					ShaderProgram* shaderProgramToUse = models[i]->GetMaterial()->GetShaderProgram();
					glUseProgram(shaderProgramToUse->GetID());

					switch (models[i]->GetRenderMode()) {
					case RenderMode::SOLID: {
						renderSubroutine = "materialColor";

						shaderProgramToUse->SetUniform3fv("Ka", models[i]->GetMaterial()->GetAmbientColor());
						shaderProgramToUse->SetUniform3fv("Kd", models[i]->GetMaterial()->GetDiffuseColor());

						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						break;
					}
					case RenderMode::TEXTURE: {
						renderSubroutine = "textureColor";

						if (models[i]->GetMaterial()->IsNormalMap()) {

							shaderProgramToUse = models[i]->GetMaterial()->GetShaderProgramNormalMap();
							glUseProgram(shaderProgramToUse->GetID());

							shaderProgramToUse->SetUniformSampler2D("normalTextureSampler", 1);
							models[i]->GetMaterial()->GetTextureNormalMap()->ActivateTexture(1);
						}
						else {

							shaderProgramToUse->SetUniform3fv("Ka", models[i]->GetMaterial()->GetAmbientColor());
							shaderProgramToUse->SetUniform3fv("Kd", models[i]->GetMaterial()->GetDiffuseColor());
						}

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

#pragma region Common Uniforms

					shaderProgramToUse->SetUniform4fm("mModelViewProj", virtualCamera->GetViewProjMatrix() * models[i]->GetTransform()->GetModelMatrix());
					shaderProgramToUse->SetUniform4fm("mModelView", virtualCamera->GetViewMatrix() * models[i]->GetTransform()->GetModelMatrix());
					shaderProgramToUse->SetUniform4fm("mModelViewIT", glm::inverse(glm::transpose(virtualCamera->GetViewMatrix() * models[i]->GetTransform()->GetModelMatrix())));

					shaderProgramToUse->SetUniform3fv("Ks", models[i]->GetMaterial()->GetSpecularColor());
					shaderProgramToUse->SetUniform1f("phongExponent", models[i]->GetMaterial()->GetPhongExponent());

#pragma endregion

#pragma region Light Uniforms

					glm::mat4 viewMatrix = virtualCamera->GetViewMatrix();

					switch (sceneLights[j]->GetLightType())
					{
					case PAG::LightType::AMBIENT: {

						shaderProgramToUse->SetUniform3fv("Ia", sceneLights[j]->GetAmbient());
						lightSubroutine = "ambient";
						break;
					}
					case PAG::LightType::DIRECTIONAL: {

						glm::vec3 lightDirection = sceneLights[j]->GetDirection();
						shaderProgramToUse->SetUniform3fv("lightDirection", glm::vec3(glm::transpose(glm::inverse(viewMatrix)) * glm::vec4(lightDirection, 0.0)));
						shaderProgramToUse->SetUniform3fv("Id", sceneLights[j]->GetDiffuse());
						shaderProgramToUse->SetUniform3fv("Is", sceneLights[j]->GetSpecular());
						lightSubroutine = "directional";

						glm::mat4 mShadow = glm::scale(glm::vec3(.5, .5, .5));
						mShadow[3][0] = mShadow[3][1] = mShadow[3][2] = 0.5;
						mShadow = mShadow * sceneLights[j]->GetLightVisionProjMatrix() * models[i]->GetTransform()->GetModelMatrix();
						shaderProgramToUse->SetUniform4fm("mShadow", mShadow);

						// Coger la textura, bindearla y activarla.
						// Pasar uniform textura al shader program.
						glActiveTexture(GL_TEXTURE2); // Activamos unidad de textura
						glBindTexture(GL_TEXTURE_2D, sceneLights[j]->GetDepthTexture()); // Asociamos la textura del FB
						shaderProgramToUse->SetUniformSampler2D("shadowSampler", 2);

						break;
					}
					case PAG::LightType::POINT: {

						shaderProgramToUse->SetUniform3fv("lightPosition", glm::vec3(viewMatrix * glm::vec4(sceneLights[j]->GetPosition(), 1.0)));
						shaderProgramToUse->SetUniform3fv("Id", sceneLights[j]->GetDiffuse());
						shaderProgramToUse->SetUniform3fv("Is", sceneLights[j]->GetSpecular());
						lightSubroutine = "point";
						break;
					}
					case PAG::LightType::SPOT: {

						shaderProgramToUse->SetUniform3fv("lightPosition", glm::vec3(viewMatrix * glm::vec4(sceneLights[j]->GetPosition(), 1.0)));
						shaderProgramToUse->SetUniform3fv("lightDirection", glm::vec3(glm::transpose(glm::inverse(viewMatrix)) * glm::vec4(sceneLights[j]->GetDirection(), 0.0)));
						shaderProgramToUse->SetUniform1f("spotlightAngle", sceneLights[j]->GetSpotlightAngle());
						shaderProgramToUse->SetUniform3fv("Id", sceneLights[j]->GetDiffuse());
						shaderProgramToUse->SetUniform3fv("Is", sceneLights[j]->GetSpecular());
						lightSubroutine = "spot";

						glm::mat4 mShadow = glm::scale(glm::vec3(.5, .5, .5));
						mShadow[3][0] = mShadow[3][1] = mShadow[3][2] = 0.5;
						mShadow = mShadow * sceneLights[j]->GetLightVisionProjMatrix() * models[i]->GetTransform()->GetModelMatrix();
						shaderProgramToUse->SetUniform4fm("mShadow", mShadow);

						glActiveTexture(GL_TEXTURE2); // Activamos unidad de textura
						glBindTexture(GL_TEXTURE_2D, sceneLights[j]->GetDepthTexture()); // Asociamos la textura del FB
						shaderProgramToUse->SetUniformSampler2D("shadowSampler", 2);

						break;
					}
					}

#pragma endregion

					glUseProgram(shaderProgramToUse->GetID());
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

void PAG::Renderer::RecalculateShadowMaps()
{
	for (int i = 0; i < sceneLights.size(); i++) {
		if (sceneLights[i]->CastShadows()) {

			GLuint depthTex = sceneLights[i]->GetDepthTexture();

			glActiveTexture(GL_TEXTURE2); // Activamos unidad de textura
			glBindTexture(GL_TEXTURE_2D, depthTex); // Asociamos la textura del FBO

			glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D, depthTex, 0);

			glReadBuffer(GL_NONE); // NO necesitamos información de color; sólo la
			glDrawBuffer(GL_NONE); // profundidad

			GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (result != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Framebuffer for shadows is not complete." << std::endl;
			}

			// Sólo borramos profundidad. Ignoramos color
			glClear(GL_DEPTH_BUFFER_BIT);

			// IMPORTANTE: el viewport tiene que tener las dimensiones del mapa de
			// sombras, Y LUEGO HAY QUE DEVOLVERLO A SU TAMAÑO CUANDO SE VAYA A
			// RENDERIZAR LA ESCENA NORMALMENTE
			glViewport(0, 0, 1024, 1024);
			// ¡OJO! Cambiamos la función del Z-buffer
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			// Para evitar el “shadow acne”, eliminamos caras delanteras
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			// Tomamos la matriz de visión y proyección de la cámara asociada a la luz
			// (mVP)
			for (int j = 0; j < models.size(); j++) {

				glm::mat4 mVP = sceneLights[i]->GetLightVisionProjMatrix();

				// Tomar la matriz de modelado del modelo actual (mM)
				glm::mat4 mModelViewProj = mVP * models[j]->GetTransform()->GetModelMatrix();

				// Aplicar matrizMVP como uniform para el shader program de cálculo de mapas de sombra
				glUseProgram(shaderProgramShadowMap->GetID());
				shaderProgramShadowMap->SetUniform4fm("mModelViewProj", mModelViewProj);

				// Renderizar los triángulos del modelo actual
				models[j]->RenderModel();
			}
		}
	}

	// IMPORTANTE: Vuelve a activar el frame buffer del sistema de ventanas
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// IMPORTANTE 2: para renderizar la escena normalmente, hay que volver a
	// cambiar la función del Z-buffer, poniéndola en GL_LEQUAL
	glDepthFunc(GL_LEQUAL);

	// IMPORTANTE 3: hay que volver a activar el dibujado de las caras delanteras.
	glDisable(GL_CULL_FACE);
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
			material->SetNormalMapTexture(new Texture(".\\textures\\NormalMap.png"));
			material->SetShaderProgramNormalMap("vs-normal", "fs-normal");
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

	needRecalculateShadowMap = true;
}

void PAG::Renderer::DeleteModel()
{
	if (activeModel != -1) {
		models.erase(models.begin() + activeModel);
		SwitchActiveModel();
	}

	needRecalculateShadowMap = true;
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

	needRecalculateShadowMap = true;
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

