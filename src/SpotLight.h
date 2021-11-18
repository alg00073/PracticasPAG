#pragma once
#include "Light.h"

namespace PAG {

	class SpotLight : public Light
	{

	private:

		glm::vec3 position;
		glm::vec3 direction;

		glm::vec3 diffuseIntensity;
		glm::vec3 specularIntensity;

		float spotlightAngle;

	public:

		SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 Id, glm::vec3 Is, float angle);
		~SpotLight();

		glm::vec3 GetDiffuse();
		glm::vec3 GetSpecular();
		glm::vec3 GetPosition();
		glm::vec3 GetDirection();
		float GetSpotlightAngle();

	};
}

