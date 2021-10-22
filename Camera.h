#pragma once
#include "glm/glm.hpp"
#include <glm/ext/vector_relational.hpp> // Para equal

class Camera
{
private:
	glm::vec3 position; // Posición de la cámara
	glm::vec3 lookAt; // Punto al que mira la cámara

	glm::vec3 u, v, n; // Sistemas de coordenadas local UVN

	float fovX, fovY; // Ángulos de visión en horizontal y vertical

	float nearZ, farZ; // Planos Z near y Z far

	int height, width; // Alto y ancho del viewport
	float aspect; // Relacion de aspecto entre altura y anchura (width / height)

public:

	Camera(glm::vec3 position, glm::vec3 lookAt, float fovX, float nearZ, float farZ, int height, int width);
	void Pan(float angle);
	void Tilt(float angle);
	void Dolly(float x, float z);
	void Crane(float y);
	void Orbit(float angleLat, float angleLong);
	void Zoom(float angle);
};

