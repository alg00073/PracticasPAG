#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 mModelViewProj;

out vec4 rainbowColor;

void main ()
{
	gl_Position = mModelViewProj * vec4(position, 1.0);
	rainbowColor = vec4 (color, 1.0);
}