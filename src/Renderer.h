#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Model.h"
#include "Camera.h"
#include "Light.h"

namespace PAG {

	class Renderer
	{

	private:
		static Renderer* instance;

		double clearColor[4] = { 0.6, 0.6, 0.6, 1 };

		// Models attributes
		int activeModel = 0;
		std::vector<Model*> models;
		TransformMode activeTransformMode = PAG::TransformMode::TRANSLATE;

		// Camera attributes
		Camera* virtualCamera;
		CameraMovementType activeMovementType = PAG::CameraMovementType::ORBIT;

		// Lights attributes
		std::vector<Light*> sceneLights;

		// Shadow Mapping attributes
		bool needRecalculateShadowMap = true;
		GLuint shadowFBO;
		ShaderProgram* shaderProgramShadowMap = nullptr;

		Renderer();

	public:
		virtual ~Renderer();
		static Renderer* Instance();
		void InitializeOpenGL();
		void InitializeShadowMapping();
		double* GetClearColor();
		void SetClearColor(double* color);
		void Refresh();
		void Resize(int width, int height);
		void ShoutInfo();
		void RecalculateShadowMaps();

		// Models methods
		void SwitchRenderMode();
		int SwitchActiveModel();
		void AddModel(int model);
		void DeleteModel();
		std::string SwitchTransformMode();
		void ApplyTransform(glm::vec3 deltaTransform);

		// Camera methods
		void ChangeCameraMovement(PAG::CameraMovementType type);
		void ApplyCameraMovement(double deltaX, double deltaY);
		void ResetCamera();
	};
}

