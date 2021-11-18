#include "AmbientLight.h"

PAG::AmbientLight::AmbientLight(glm::vec3 Ia): Light(LightType::AMBIENT), ambientIntensity(Ia)
{
}

PAG::AmbientLight::~AmbientLight()
{
}

glm::vec3 PAG::AmbientLight::GetAmbient()
{
	return ambientIntensity;
}
