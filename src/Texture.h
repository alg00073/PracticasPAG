#pragma once

#include <iostream>
#include "lodepng.h"

namespace PAG {
	class Texture
	{
	private:
		GLuint id;

		std::vector<unsigned char> image;
		GLuint width, height;

		void Load(std::string texturePath);

	public:

		Texture(std::string texturePath);
		~Texture();

		GLuint GetID();

		void ActivateTexture(GLint textureLevel);
	};
}
