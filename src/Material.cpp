#include "Material.h"

PAG::Material::Material(glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float Ns): Ia(Ia), Id(Id), Is(Is), Ns(Ns)
{
}

PAG::Material::Material(const Material& other): Ia(other.Ia), Id(other.Id), Is(other.Is), Ns(other.Ns)
{
}

glm::vec3 PAG::Material::GetDiffuseColor()
{
    return Id;
}
