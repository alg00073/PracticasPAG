#include "Light.h"

PAG::Light::Light(LightType type) : lightType(type)
{
}

PAG::Light::~Light()
{
}

PAG::LightType PAG::Light::GetLightType()
{
    return lightType;
}
