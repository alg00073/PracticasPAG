#include "PointLight.h"

PAG::PointLight::PointLight(glm::vec3 pos, glm::vec3 Id, glm::vec3 Is): Light(LightType::POINT),
position(pos), diffuseIntensity(Id), specularIntensity(Is)
{
}

PAG::PointLight::~PointLight()
{
}

glm::vec3 PAG::PointLight::GetDiffuse()
{
	return diffuseIntensity;
}

glm::vec3 PAG::PointLight::GetSpecular()
{
	return specularIntensity;
}

glm::vec3 PAG::PointLight::GetPosition()
{
	return position;
}
