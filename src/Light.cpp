#define GLM_ENABLE_EXPERIMENTAL

#include "Light.h"
#include <glm/gtx/transform.hpp>
#include <glm/ext/vector_relational.hpp> // Para equal
#include <iostream>

PAG::Light::Light(glm::vec3 Ia): lightType(LightType::AMBIENT), ambientIntensity(Ia), castShadows(false)
{
}

PAG::Light::Light(glm::vec3 vec, glm::vec3 Id, glm::vec3 Is, LightType type): lightType(type), diffuseIntensity(Id), specularIntensity(Is)
{
	GLfloat border[] = { 1.0, 1.0, 1.0, 1.0 };

	switch (type) {
	case LightType::POINT:
		position = vec;
		castShadows = false;
		break;
	case LightType::DIRECTIONAL:
		direction = vec;
		castShadows = true;

		// Initialize shadow Depth Texture
		glGenTextures(1, &depthTex);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		break;
	default: 
		throw std::runtime_error("Light() -> This constructor is only for Point and Directional lights");
	}
}


PAG::Light::Light(glm::vec3 pos, glm::vec3 dir, glm::vec3 Id, glm::vec3 Is, float angle): lightType(PAG::LightType::SPOT),
position(pos), direction(dir), diffuseIntensity(Id), specularIntensity(Is), spotlightAngle(angle), castShadows(true)
{
	GLfloat border[] = { 1.0, 1.0, 1.0, 1.0 };

	// Initialize shadow Depth Texture
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
}

PAG::Light::~Light()
{
	if (castShadows) {
		glDeleteTextures(1, &depthTex);
	}
}

PAG::LightType PAG::Light::GetLightType()
{
	return lightType;
}

glm::vec3 PAG::Light::GetPosition()
{
	return position;
}

glm::vec3 PAG::Light::GetDirection()
{
	return direction;
}

float PAG::Light::GetSpotlightAngle()
{
	return spotlightAngle;
}

glm::vec3 PAG::Light::GetAmbient()
{
	return ambientIntensity;
}

glm::vec3 PAG::Light::GetDiffuse()
{
	return diffuseIntensity;
}

glm::vec3 PAG::Light::GetSpecular()
{
	return specularIntensity;
}

bool PAG::Light::CastShadows()
{
	return castShadows;
}

GLuint PAG::Light::GetDepthTexture()
{
	if (castShadows) {
		return depthTex;
	}
}

glm::mat4 PAG::Light::GetLightVisionProjMatrix()
{
	glm::vec3 n = glm::normalize(glm::vec3(5.0) * (-direction) - glm::vec3(0, 0, 0));

	glm::vec3 Y = glm::vec3(0, 1, 0);
	glm::vec3 Z = glm::vec3(0, 0, 1);

	glm::vec3 u = glm::cross(Y, n);

	// Caso especial: Si Y y n son colineales
	if (glm::all(equal(u, glm::vec3(0, 0, 0), 0.001f))) {

		if (glm::all(equal(n, Y, 0.001f))) {
			u = glm::cross(Z, n);
		}
		else if (glm::all(equal(n, -Y, 0.001f))) {
			u = glm::cross(-Z, n);
		}
		else {
			throw std::runtime_error("Light::GetLightVisionProjMatrix() -> No se pudo asignar el vector U correctamente.");
		}
	}

	glm::vec3 v = glm::cross(n, u);

	if (lightType == LightType::DIRECTIONAL) {
		glm::mat4 mVision = glm::lookAt(glm::vec3(5.0) * (-direction), glm::vec3(0, 0, 0), v);
		glm::mat4 mProj = glm::ortho(-3.0, 3.0, -3.0, 3.0, 0.1, 10.0);

		return mProj * mVision;
	}
	else {
		glm::mat4 mVision = glm::lookAt(position, position + direction, v);
		glm::mat4 mProj = glm::perspective(2 * glm::radians(spotlightAngle), (float)1024 / (float)1024, 0.1f, 10.0f);

		return mProj * mVision;
	}

	throw std::runtime_error("Light::GetLightVisionProjMatrix() -> Cannot calculate matrix for this lightType");
}
