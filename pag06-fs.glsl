#version 410

in vec4 finalColor;

layout (location = 0) out vec4 fragColor;

subroutine vec4 calcularColor();
subroutine uniform calcularColor colorElegido;

subroutine(calcularColor)
vec4 colorWireframe()
{
  return (vec4 (1,0,0,1));
}

subroutine(calcularColor)
vec4 colorSolid()
{
  return finalColor;
}

void main ()
{
  fragColor = colorElegido();
}