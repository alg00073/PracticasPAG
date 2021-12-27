#pragma once
#include "glm/glm.hpp"
#include <GL/glew.h>

namespace PAG {

	enum class LightType
	{
		AMBIENT,
		POINT,
		SPOT,
		DIRECTIONAL
	};

	class Light
	{
	private:

		LightType lightType;
		bool castShadows;

		glm::vec3 position;
		glm::vec3 direction;
		float spotlightAngle;

		glm::vec3 ambientIntensity;
		glm::vec3 diffuseIntensity;
		glm::vec3 specularIntensity;

		// Shadow Map
		GLuint depthTex;

	public:

		Light(glm::vec3 Ia); // Ambient Light constructor
		Light(glm::vec3 vec, glm::vec3 Id, glm::vec3 Is, LightType type); // Point/Directional Light constructor
		Light(glm::vec3 pos, glm::vec3 dir, glm::vec3 Id, glm::vec3 Is, float angle); // Spot Light constructor

		virtual ~Light();

		LightType GetLightType();
		
		glm::vec3 GetPosition();
		glm::vec3 GetDirection();
		float GetSpotlightAngle();

		glm::vec3 GetAmbient();
		glm::vec3 GetDiffuse();
		glm::vec3 GetSpecular();

		bool CastShadows();
		GLuint GetDepthTexture();
		glm::mat4 GetLightVisionProjMatrix();
	};
}

