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

	class Renderer
	{

	private:
		static Renderer* instance;

		double clearColor[4] = { 0.6, 0.6, 0.6, 1 };

		int activeModel = 0;
		std::vector<Model*> models;

		Camera* virtualCamera;
		MovementType activeMovementType = PAG::MovementType::ORBIT;

		std::vector<Light*> sceneLights;

		Renderer();

	public:
		virtual ~Renderer();
		static Renderer* Instance();
		void InitializeOpenGL();
		double* GetClearColor();
		void SetClearColor(double* color);
		void Refresh();
		void Resize(int width, int height);
		void ShoutInfo();
		int SwitchActiveModel();
		void AddModel();
		void DeleteModel();
		void ChangeCameraMovement(PAG::MovementType type);
		void ApplyCameraMovement(double deltaX, double deltaY);
		void ResetCamera();
		void SwitchRenderMode();
	};
}

