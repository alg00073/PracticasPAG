#version 410

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

uniform mat4 mModelViewProj;
uniform mat4 mModelView;

out vec3 position;
out vec3 normal;

void main ()
{
	gl_Position = mModelViewProj * vec4(vPosition, 1.0);

	vec4 position4v = mModelView * vec4(vPosition, 1.0);
	vec4 normal4v = mModelView * vec4(vPosition, 1.0);

	position = position4v.xyz;
	normal = normal4v.xyz;
}