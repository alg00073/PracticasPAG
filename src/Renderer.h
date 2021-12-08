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
		TransformMode activeTransformMode = PAG::TransformMode::TRANSLATE;

		Camera* virtualCamera;
		CameraMovementType activeMovementType = PAG::CameraMovementType::ORBIT;

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

		void SwitchRenderMode();
		int SwitchActiveModel();
		void AddModel(int model);
		void DeleteModel();
		std::string SwitchTransformMode();
		void ApplyTransform(glm::vec3 deltaTransform);

		void ChangeCameraMovement(PAG::CameraMovementType type);
		void ApplyCameraMovement(double deltaX, double deltaY);
		void ResetCamera();
	};
}

