#pragma once

#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

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