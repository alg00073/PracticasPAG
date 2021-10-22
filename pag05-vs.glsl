#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec4 finalColor;

void main ()
{
	gl_Position = vec4 ( position, 1 );
	finalColor = vec4 (color, 1.0);
}