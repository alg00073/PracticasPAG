#pragma once
#include "glm/glm.hpp"

namespace PAG {

	enum class TransformMode {
		TRANSLATE,
		ROTATE,
		SCALE
	};

	class Transform
	{
	private:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	public:
		Transform();
		~Transform();
		glm::vec3 GetPosition();
		glm::vec3 GetRotation();
		glm::vec3 GetScale();
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::vec3 rotation);
		void SetScale(glm::vec3 scale);
		glm::mat4 GetModelMatrix();
	};
}

