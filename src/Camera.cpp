#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"
#include <stdexcept>
#include <glm/gtx/transform.hpp>
#include <iostream>

PAG::Camera::Camera(glm::vec3 position, glm::vec3 lookAt, float fovX, float nearZ, float farZ, int height, int width) :
	position(position), lookAt(lookAt), fovX(fovX), zNear(nearZ), zFar(farZ), height(height), width(width)
{
	RecalculateCamera();
}

glm::mat4 PAG::Camera::GetModelViewProjMatrix()
{
	try {
		RecalculateCamera();
	}
	catch (std::exception& ex) {
		throw std::runtime_error("Camera::GetModelViewProjMatrix() -> Error during calculating camera parameters: " + std::string(ex.what()));
	}

	glm::mat4 view = glm::lookAt(position, lookAt, v);
	glm::mat4 projection = glm::perspective(fovY, aspect, zNear, zFar);

	return projection * view;
}

void PAG::Camera::SetHeight(int height)
{
	this->height = height;
}

void PAG::Camera::SetWidth(int width)
{
	this->width = width;
}

void PAG::Camera::RecalculateCamera()
{
	aspect = (float)width / (float)height;
	fovY = 2 * glm::atan(glm::tan(fovX / 2) / aspect);

	n = glm::normalize(position - lookAt);

	glm::vec3 Y = glm::vec3(0, 1, 0);
	glm::vec3 Z = glm::vec3(0, 0, 1);

	u = glm::cross(Y, n);

	// Caso especial: Si Y y n son colineales
	if (glm::all(equal(u, glm::vec3(0, 0, 0), 0.001f))) {

		if (glm::all(equal(n, Y, 0.001f))) {
			u = glm::cross(Z, n);
		}
		else if (glm::all(equal(n, -Y, 0.001f))) {
			u = glm::cross(-Z, n);
		}
		else {
			throw std::runtime_error("Camera::RecalculateCamera() -> No se pudo asignar el vector U correctamente.");
		}
	}

	v = glm::cross(n, u);
}

void PAG::Camera::ApplyMovement(double deltaX, double deltaY, MovementType type)
{
	switch (type)
	{
	case PAG::MovementType::PAN:
		Pan(deltaX);
		break;
	case PAG::MovementType::TILT:
		Tilt(deltaY);
		break;
	case PAG::MovementType::DOLLY:
	{
		double translateX = deltaX / 100;
		double translateZ = deltaY / 100;

		Dolly(translateX, translateZ);
		break;
	}
	case PAG::MovementType::CRANE:
	{
		double translateY = deltaY / 100;

		Crane(translateY);
		break;
	}
	case PAG::MovementType::ORBIT:
		Orbit(deltaX, deltaY);
		break;
	case PAG::MovementType::ZOOM:
	{
		double zoomFactor = deltaY / 100;
		Zoom(zoomFactor);
		break;
	}
	default:
		break;
	}

	RecalculateCamera();
}

void PAG::Camera::Reset()
{
	position = glm::vec3(0, 0, 2);
	lookAt = glm::vec3(0, 0, 0);
	fovX = glm::radians(60.0f);

	RecalculateCamera();
}

void PAG::Camera::Pan(float angle)
{
	glm::mat4 pan = glm::rotate(glm::radians(angle), v);

	glm::mat4 transformation = glm::translate(glm::mat4(1.0f), position) * pan * glm::translate(glm::mat4(1.0f), -position);

	lookAt = transformation * glm::vec4(lookAt, 1.0f);
}

void PAG::Camera::Tilt(float angle)
{
	glm::mat4 tilt = glm::rotate(glm::radians(angle), u);

	glm::mat4 transformation = glm::translate(glm::mat4(1.0f), position) * tilt * glm::translate(glm::mat4(1.0f), -position);

	lookAt = transformation * glm::vec4(lookAt, 1.0f);
}

void PAG::Camera::Dolly(float x, float z)
{
	glm::vec3 dolly(x, 0, z);

	glm::mat4 transformation = glm::translate(dolly); // Traslado

	position = transformation * glm::vec4(position, 1.0f); // Aplico la transformación
	lookAt = transformation * glm::vec4(lookAt, 1.0f);
}

void PAG::Camera::Crane(float y)
{
	glm::vec3 crane(0, y, 0);

	glm::mat4 transformation = glm::translate(crane);

	position = transformation * glm::vec4(position, 1.0f);
	lookAt = transformation * glm::vec4(lookAt, 1.0f);
}

void PAG::Camera::Orbit(float angleLong, float angleLat)
{
	glm::mat4 rotationLong = glm::rotate(glm::radians(angleLong), v);
	glm::mat4 rotationLat = glm::rotate(glm::radians(angleLat), u);

	glm::mat4 transformation = glm::translate(lookAt) * rotationLong * rotationLat * glm::translate(-lookAt);

	position = transformation * glm::vec4(position, 1.0f);
}

void PAG::Camera::Zoom(float angle)
{
	fovX += angle;

	fovX = glm::clamp(fovX, 0.01f, 2.0f);

	std::cout << fovX << std::endl;
}
