#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

namespace PAG {

	enum class RenderMode {
		SOLID,
		WIREFRAME
	};

	class Renderer
	{

	private:
		static Renderer* instance;

		RenderMode activeRenderMode = PAG::RenderMode::SOLID;

		double clearColor[4] = { 0.6, 0.6, 0.6, 1 };

		int activeModel = 0;
		Model* model;

		Camera* virtualCamera;
		MovementType activeMovementType = PAG::MovementType::ORBIT;

		std::vector<Light*> sceneLights;

		Renderer();
		Model* CreateTriangle();
		Model* CreateTetrahedron();

	public:
		virtual ~Renderer();
		static Renderer* Instance();
		void InitializeOpenGL();
		double* GetClearColor();
		void SetClearColor(double* color);
		void Refresh();
		void Resize(int width, int height);
		void ShoutInfo();
		void SwitchModel();
		void DeleteModel();
		void ChangeCameraMovement(PAG::MovementType type);
		void ApplyCameraMovement(double deltaX, double deltaY);
		void ResetCamera();
		void ChangeRenderMode(PAG::RenderMode mode);

		void SetUniform1f(std::string name, float data);
		void SetUniform3fv(std::string name, glm::vec3 data);
		void SetUniform4fm(std::string name, glm::mat4 data);
		void SetSubroutineUniform(std::string name);
	};
}

