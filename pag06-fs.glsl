#version 410

in vec4 finalColor;

layout (location = 0) out vec4 fragColor;

void main ()
{
  fragColor = finalColor;
}