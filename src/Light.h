#pragma once
#include "glm/glm.hpp"

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

