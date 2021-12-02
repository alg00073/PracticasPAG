#include "Texture.h"

#include <GL/glew.h> // En algunos casos, este include se puede omitir
#include <GL/gl.h>

PAG::Texture::Texture(std::string texturePath) : width(0), height(0), image(std::vector<unsigned char>()), id(0)
{
	try {
		Load(texturePath);
	}
	catch (std::exception& ex) {
		throw std::runtime_error("Texture() -> " + (std::string)ex.what());
	}
}

PAG::Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

GLuint PAG::Texture::GetID()
{
	return id;
}

void PAG::Texture::ActivateTexture(GLint textureLevel)
{
	if (image.size() <= 0) {
		throw std::runtime_error("Texture::ActivateTexture() -> No image loaded");
	}

	switch (textureLevel)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	default:
		break;
	}

	glBindTexture(GL_TEXTURE_2D, id);
}

void PAG::Texture::Load(std::string texturePath)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	GLuint error = lodepng::decode(image, width, height, texturePath);

	if (error) {
		throw std::runtime_error("Texture::Load() -> Can't decode " + texturePath);
	}

	//Reverse texture image
	unsigned char* img = &image[0];
	int colorComponents = 4;
	int widthIncrement = 4 * width; //bytes
	unsigned char* top = nullptr;
	unsigned char* bot = nullptr;
	unsigned char temp = 0;

	for (int i = 0; i < height / 2; i++) {
		top = img + i * widthIncrement;
		bot = img + (height - i - 1) * widthIncrement;

		for (int j = 0; j < widthIncrement; j++) {
			temp = *top;
			*top = *bot;
			*bot = temp;
			++top;
			++bot;
		}
	}

	//Config texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

	glGenerateMipmap(GL_TEXTURE_2D);
}