#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 Id, glm::vec3 Is, float angle): Light(LightType::SPOT), 
position(pos), direction(dir), diffuseIntensity(Id), specularIntensity(Is), spotlightAngle(angle)
{
}

PAG::SpotLight::~SpotLight()
{
}

glm::vec3 PAG::SpotLight::GetDiffuse()
{
	return diffuseIntensity;
}

glm::vec3 PAG::SpotLight::GetSpecular()
{
	return specularIntensity;
}

glm::vec3 PAG::SpotLight::GetPosition()
{
	return position;
}

glm::vec3 PAG::SpotLight::GetDirection()
{
	return direction;
}

float PAG::SpotLight::GetSpotlightAngle()
{
	return spotlightAngle;
}
