#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Model.h"

namespace PAG {
	class Renderer
	{

	private:
		static Renderer* instance;

		double clearColor[4] = { 0.6, 0.6, 0.6, 1 };

		bool activeModel = true;
		Model* triangleModel;

		Renderer();
		Model* CreateTriangle();

	public:
		virtual ~Renderer();
		static Renderer* Instance();
		void InitializeOpenGL();
		double* GetClearColor();
		void SetClearColor(double* color);
		void Refresh();
		void Resize(int width, int height);
		void EnableDepth();
		void ShoutInfo();
		void AddModel();
		void DeleteModel();
	};
}

