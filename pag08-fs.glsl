#version 410

in vec3 position;
in vec3 normal;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;

uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

uniform float phongExponent;

uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float spotlightAngle;

layout (location = 0) out vec4 fragColor;

subroutine vec4 calculateLight();
subroutine uniform calculateLight lightType;

void main ()
{
  fragColor = calculateLight();
}

subroutine(calculateLight)
vec3 ambient()
{
  vec3 ambient = (Ia * Ka);
  return ambient;
}

subroutine(calculateLight)
vec3 point()
{
  vec3 n = normalize(normal);

  vec3 l = normalize(lightPosition - position);
  vec3 v = normalize(-position);
  vec3 r = reflect(-l, n);

  vec3 diffuse = (Id * Kd * max(dot(l,n), 0.0));
  vec3 specular = (Is * Ks * pow(max(dot(r,v), 0.0), shininess));

  return diffuse + specular;
}

subroutine(calculateLight)
vec3 directional()
{
  vec3 n = normalize(normal);

  vec3 l = normalize(-lightDirection);
  vec3 v = normalize(-position);
  vec3 r = reflect(-l, n);

  vec3 diffuse = (Id * Kd * max(dot(l,n), 0.0));
  vec3 specular = (Is * Ks * pow(max(dot(r,v), 0.0), shininess));

  return diffuse + specular;
}

subroutine(calculateLight)
vec3 spot()
{
  vec3 l = normalize(lightPosition - position);
  vec3 d = lightDirection;
  float cosGamma = cos(spotlightAngle * 180.0 / M_PI);
  float spotlightFactor = 1.0 

  if (dot(-l, d) < cosGamma) { spotlightFactor = 0.0; }

  vec3 n = normalize(normal);
  vec3 v = normalize(-position);
  vec3 r = reflect(-l, n);

  vec3 diffuse = (Id * Kd * max(dot(l,n), 0.0));
  vec3 specular = (Is * Ks * pow(max(dot(r,v), 0.0), shininess));

  return (diffuse + specular) * spotlightFactor;
}