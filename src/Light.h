#pragma once
#include "glm/glm.hpp"

#pragma region Include LightTypes

#include "PointLight.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

#pragma endregion

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

	public:

		Light(LightType type);
		virtual ~Light();

		LightType GetLightType();

	};
}

