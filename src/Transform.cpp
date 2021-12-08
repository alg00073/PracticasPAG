#define GLM_ENABLE_EXPERIMENTAL
#include "Transform.h"
#include <glm/gtx/transform.hpp>

PAG::Transform::Transform(): position(glm::vec3(0, 0, 0)), rotation(glm::vec3(0, 0, 0)), scale(glm::vec3(1.0f))
{
}

PAG::Transform::~Transform()
{
}

glm::vec3 PAG::Transform::GetPosition()
{
	return position;
}

glm::vec3 PAG::Transform::GetRotation()
{
	return rotation;
}

glm::vec3 PAG::Transform::GetScale()
{
	return scale;
}

void PAG::Transform::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void PAG::Transform::SetRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
}

void PAG::Transform::SetScale(glm::vec3 scale)
{
	this->scale = scale;
}

glm::mat4 PAG::Transform::GetModelMatrix()
{
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

	glm::mat4 rotationX = glm::rotate(translation, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	glm::mat4 rotationY = glm::rotate(rotationX, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	glm::mat4 rotationZ = glm::rotate(rotationY, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	glm::mat4 modelMatrix = glm::scale(rotationZ, scale);

	return modelMatrix;
}
