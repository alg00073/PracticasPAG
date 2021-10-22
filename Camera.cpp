#define GLM_ENABLE_EXPERIMENTAL

#include "Camera.h"
#include <stdexcept>
#include <glm/gtx/transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 lookAt, float fovX, float nearZ, float farZ, int height, int width) :
	position(position), lookAt(lookAt), fovX(fovX), nearZ(nearZ), farZ(farZ), height(height), width(width)
{
	aspect = width / height;
	fovY = 2 * glm::atan(glm::tan(fovX / 2) / aspect);

	n = glm::normalize(position - lookAt);

	glm::vec3 Y = glm::vec3(0, 1, 0);
	glm::vec3 Z = glm::vec3(0, 0, 1);

	u = glm::cross(Y, n);

	// Si Y y n son colineales
	if (glm::all(equal(u, glm::vec3(0, 0, 0), 0.001f))) {

		if (glm::all(equal(u, Y, 0.001f))) {
			u = glm::cross(Z, n);
		}
		else if (glm::all(equal(u, -Y, 0.001f))) {
			u = glm::cross(-Z, n);
		}
		else {
			throw std::runtime_error("Camera::Camera() -> No se pudo asignar el vector U correctamente.");
		}
	}

	v = glm::cross(n, u);
}

void Camera::Pan(float angle)
{
	glm::mat4 m = glm::translate(-lookAt); // Traslado al origen
	m = glm::rotate(angle, v); // Hago la rotación
	m = glm::translate(lookAt); // Traslado de nuevo al punto

	lookAt = m * glm::vec4(lookAt, 0); // Aplico la transformación
}

void Camera::Tilt(float angle)
{
	glm::mat4 m = glm::translate(lookAt - position); // Traslado lookAt a la posición de la camara
	m = glm::rotate(angle, u); // Hago la rotación
	m = glm::translate(position - lookAt); // Traslado de nuevo al punto original

	lookAt = m * glm::vec4(lookAt, 0); // Aplico la transformación
}

void Camera::Dolly(float x, float z)
{
	glm::vec3 t(x, 0, z);

	glm::mat4 m = glm::translate(t); // Traslado

	position = m * glm::vec4(position, 0); // Aplico la transformación
	lookAt = m * glm::vec4(lookAt, 0);
}

void Camera::Crane(float y)
{
	glm::vec3 t(0, y, 0);

	glm::mat4 m = glm::translate(t); // Traslado

	position = m * glm::vec4(position, 0); // Aplico la transformación
	lookAt = m * glm::vec4(lookAt, 0);
}

void Camera::Orbit(float angleLat, float angleLong)
{
	glm::mat4 m = glm::translate(position - lookAt); // Traslado la posición de la camara al punto lookAt
	m = glm::rotate(angleLat, u); // Hago la rotación en latitud
	m = glm::rotate(angleLong, v); // Hago la rotación en longitud
	m = glm::translate(lookAt - position); // Traslado de nuevo a la posición original

	lookAt = m * glm::vec4(lookAt, 0); // Aplico la transformación
}

void Camera::Zoom(float angle)
{
	fovX += angle;
}
