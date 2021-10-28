#pragma once
#include "glm/glm.hpp"

namespace PAG {
	class Material
	{
	private:
		glm::vec3 Ia; // Color ambiente
		glm::vec3 Id; // Color difuso
		glm::vec3 Is; // Color especular

		float Ns; // Exponente de phong (brillo)
	public:
		Material(glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float Ns);
	};
}

