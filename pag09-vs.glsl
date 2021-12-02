#version 410

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 mModelViewProj;
uniform mat4 mModelView;
uniform mat4 mModelViewIT;

out vec3 position;
out vec3 normal;
out vec2 texCoord;

void main ()
{
	gl_Position = mModelViewProj * vec4(vPosition, 1.0);

	normal = vec3( mModelView * vec4(vNormal, 0.0) );
 	position = vec3( mModelView * vec4(vPosition, 1.0) ); 
	texCoord = vTexCoord;
}