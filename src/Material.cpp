#include "Material.h"

PAG::Material::Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns): 
    ambientColor(Ka), diffuseColor(Kd), specularColor(Ks), phongExponent(Ns)
{
}

PAG::Material::Material(const Material& other): ambientColor(other.ambientColor), diffuseColor(other.diffuseColor), specularColor(other.specularColor), 
phongExponent(other.phongExponent)
{
    shaderProgram = new ShaderProgram(*other.shaderProgram);
    shaderProgramNormalMap = new ShaderProgram(*other.shaderProgramNormalMap);
}

PAG::Material::~Material()
{
    delete shaderProgram;
    delete shaderProgramNormalMap;

    delete texture;
    delete textureNormalMap;
    
    shaderProgram = shaderProgramNormalMap = nullptr;
    texture = textureNormalMap = nullptr;
}

void PAG::Material::SetTexture(Texture* texture)
{
    this->texture = texture;
}

PAG::Texture* PAG::Material::GetTexture()
{
    return texture;
}

void PAG::Material::SetNormalMapTexture(Texture* textureNormalMap)
{
    this->textureNormalMap = textureNormalMap;
}

PAG::Texture* PAG::Material::GetTextureNormalMap()
{
    return textureNormalMap;
}

void PAG::Material::SetShaderProgram(std::string vertexShader, std::string fragmentShader)
{
    try {
        shaderProgram = new ShaderProgram(vertexShader, fragmentShader);
    }
    catch (std::exception& ex) {
        throw std::runtime_error("Material::SetShaderProgram() -> " + (std::string)ex.what());
    }
}

PAG::ShaderProgram* PAG::Material::GetShaderProgram()
{
    return shaderProgram;
}

void PAG::Material::SetShaderProgramNormalMap(std::string vertexShader, std::string fragmentShader)
{
    try {
        hasNormalMap = true;
        shaderProgramNormalMap = new ShaderProgram(vertexShader, fragmentShader);
    }
    catch (std::exception& ex) {
        throw std::runtime_error("Material::SetShaderProgramNormalMap() -> " + (std::string)ex.what());
    }
}

PAG::ShaderProgram* PAG::Material::GetShaderProgramNormalMap()
{
    return shaderProgramNormalMap;
}

bool PAG::Material::IsNormalMap()
{
    return hasNormalMap;
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
