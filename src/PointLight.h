#pragma once
#include "Light.h"

namespace PAG {

	class PointLight : public Light
	{
	private:

		glm::vec3 position;

		glm::vec3 diffuseIntensity;
		glm::vec3 specularIntensity;

	public:

		PointLight(glm::vec3 pos, glm::vec3 Id, glm::vec3 Is);
		~PointLight();

		glm::vec3 GetDiffuse();
		glm::vec3 GetSpecular();
		glm::vec3 GetPosition();
	};
}

