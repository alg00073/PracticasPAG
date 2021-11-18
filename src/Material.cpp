#include "Material.h"

PAG::Material::Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns): 
    ambientColor(Ka), diffuseColor(Kd), specularColor(Ks), phongExponent(Ns)
{
}

PAG::Material::Material(const Material& other): ambientColor(other.ambientColor), diffuseColor(other.diffuseColor), specularColor(other.specularColor), phongExponent(other.phongExponent)
{
}

glm::vec3 PAG::Material::GetAmbientColor()
{
    return ambientColor;
}

glm::vec3 PAG::Material::GetDiffuseColor()
{
    return diffuseColor;
}

glm::vec3 PAG::Material::GetSpecularColor()
{
    return specularColor;
}

float PAG::Material::GetPhongExponent()
{
    return phongExponent;
}
