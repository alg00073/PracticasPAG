#pragma once

#include <vector>
#include <string>

#include "src/lodepng.h"

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

		static Texture* CreateTexture(std::string texturePath);

		GLuint GetID();

		void ActivateTexture(GLint textureLevel);
	};
}
