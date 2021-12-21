#version 410

#define M_PI 3.1415926535897932384626433832795

in vec3 tangentPosition;
in vec2 texCoord;

in vec3 lightPosTangent;
in vec3 lightDirTangent;

uniform vec3 Ks;

uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;

uniform float phongExponent;    
uniform float spotlightAngle;

uniform sampler2D textureSampler;
uniform sampler2D normalTextureSampler;

layout (location = 0) out vec4 fragColor;

subroutine vec3 calculateLight(vec3 colorKa, vec3 colorKd, vec4 normal);
subroutine uniform calculateLight lightType;

subroutine vec4 chooseColor();
subroutine uniform chooseColor colorType;

void main ()
{
  fragColor = colorType();
}

subroutine (calculateLight)
vec3 ambient(vec3 colorKa, vec3 colorKd, vec4 normal)
{
  vec3 ambient = (Ia * colorKa);
  return ambient;
}

subroutine (calculateLight)
vec3 point(vec3 colorKa, vec3 colorKd, vec4 normal)
{
  vec3 n = normalize(normal.rgb);

  vec3 l = normalize(lightPosTangent - tangentPosition);
  vec3 v = normalize(-tangentPosition);
  vec3 r = reflect(-l, n);

  vec3 diffuse = (Id * colorKd * max( dot(l, n), 0.0 ) );
  vec3 specular = (Is * Ks * pow( max( dot(r, v), 0.0), phongExponent));

  return diffuse + specular;
}

subroutine (calculateLight)
vec3 directional(vec3 colorKa, vec3 colorKd, vec4 normal)
{
  vec3 n = normalize(normal.rgb);

  vec3 l = normalize(-lightDirTangent);
  vec3 v = normalize(-tangentPosition);
  vec3 r = reflect(-l, n);

  vec3 diffuse = Id * colorKd * max( dot(l, n), 0.0 );
  vec3 specular = Is * Ks * pow( max( dot(r, v), 0.0), phongExponent);

  return diffuse + specular;
}

subroutine (calculateLight)
vec3 spot(vec3 colorKa, vec3 colorKd, vec4 normal)
{
  vec3 l = normalize(lightPosTangent - tangentPosition);
  vec3 d = normalize(lightDirTangent);

  float cosGamma = cos(spotlightAngle * M_PI / 180.0);
  float spotlightFactor = 1.0;

  if (dot(-l, d) < cosGamma) { spotlightFactor = 0.0; }

  vec3 n = normalize(normal.rgb);
  vec3 v = -tangentPosition;
  vec3 r = reflect(-l, n);

  vec3 diffuse = (Id * colorKd * max( dot(l, n), 0.0 ) );
  vec3 specular = (Is * Ks * pow( max( dot(r, v), 0.0), phongExponent) );

  return (diffuse + specular) * spotlightFactor;
}

subroutine (chooseColor)
vec4 textureColor()
{
  vec4 normal = 2.0 * (texture(normalTextureSampler, texCoord) - 0.5); 
  vec4 color = texture(textureSampler, texCoord);

  return vec4( lightMode(color.rgb, color.rgb, normal), 1.0);
}