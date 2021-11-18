#pragma once
#include "Light.h"

namespace PAG {

	class AmbientLight : public Light
	{
	private:

		glm::vec3 ambientIntensity;

	public:

		AmbientLight(glm::vec3 Ia);
		~AmbientLight();

		glm::vec3 GetAmbient();
	};
}
