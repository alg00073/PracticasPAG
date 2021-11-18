#include "DirectionalLight.h"

PAG::DirectionalLight::DirectionalLight(glm::vec3 dir, glm::vec3 Id, glm::vec3 Is): Light(LightType::DIRECTIONAL),
direction(dir), diffuseIntensity(Id), specularIntensity(Is)
{
}

PAG::DirectionalLight::~DirectionalLight()
{
}

glm::vec3 PAG::DirectionalLight::GetDiffuse()
{
	return diffuseIntensity;
}

glm::vec3 PAG::DirectionalLight::GetSpecular()
{
	return specularIntensity;
}

glm::vec3 PAG::DirectionalLight::GetDirection()
{
	return direction;
}
