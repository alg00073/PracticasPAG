#version 410

in vec4 rainbowColor;

uniform vec3 Id;

layout (location = 0) out vec4 fragColor;

subroutine vec4 calcularColor();
subroutine uniform calcularColor colorElegido;

subroutine(calcularColor)
vec4 colorWireframe()
{
  return (vec4 (0,1,0,1));
}

subroutine(calcularColor)
vec4 colorRainbow()
{
  return rainbowColor;
}

subroutine(calcularColor)
vec4 colorSolid()
{
  return vec4 (Id, 1.0);
}

void main ()
{
  fragColor = colorElegido();
}