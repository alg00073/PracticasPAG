#pragma once
#include "Light.h"

namespace PAG {
	class DirectionalLight : public Light
	{
	private:

		glm::vec3 direction;

		glm::vec3 diffuseIntensity;
		glm::vec3 specularIntensity;

	public:

		DirectionalLight(glm::vec3 dir, glm::vec3 Id, glm::vec3 Is);
		~DirectionalLight();

		glm::vec3 GetDiffuse();
		glm::vec3 GetSpecular();
		glm::vec3 GetDirection();

	};
}

