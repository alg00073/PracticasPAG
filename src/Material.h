#pragma once
#include "glm/glm.hpp"

namespace PAG {
	class Material
	{
	private:
		glm::vec3 ambientColor; // Color ambiente
		glm::vec3 diffuseColor; // Color difuso
		glm::vec3 specularColor; // Color especular

		float phongExponent; // Exponente de phong (brillo)
	public:
		Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns);
		Material(const Material& other);

		glm::vec3 GetAmbientColor();
		glm::vec3 GetDiffuseColor();
		glm::vec3 GetSpecularColor();
		float GetPhongExponent();
	};
}

