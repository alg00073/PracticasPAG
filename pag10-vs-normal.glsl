#version 410

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

uniform mat4 mModelViewProj;
uniform mat4 mModelView;
uniform mat4 mModelViewIT;

uniform vec3 lightPosition;
uniform vec3 lightDirection;

out vec3 tangentPosition;
out vec2 texCoord;

out vec3 lightPosTangent;
out vec3 lightDirTangent;

void main ()
{	
    gl_Position = mModelViewProj * vec4 ( vPosition, 1.0 );

	vec3 modelViewTangent = normalize( vec3( mModelViewIT * vec4(vTangent, 0.0) ) );
	vec3 modelViewBitangent = normalize( vec3( mModelViewIT * vec4(vBitangent, 0.0) ) );
	vec3 normal = normalize (vec3 (mModelViewIT * vec4(vNormal, 0.0 ) ) );

	mat3 TBN = transpose(mat3(modelViewTangent, modelViewBitangent, normal));

	tangentPosition = TBN * vec3(mModelView * vec4(vPosition, 1.0));
    texCoord = vTexCoord;

	lightPosTangent = TBN * lightPosition;
	lightDirTangent = TBN * lightDirection;
}