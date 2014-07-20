layout (std140) uniform cb0 { mat4 model; mat4 modelView; mat4 modelViewProj; };
layout (std140) uniform cb1 { mat4 view; mat4 proj; };

in vec3 position;
in vec3 normal;
in vec3 binormal;
in vec2 textureCoordinate;

out vec3 vPosition;
out vec2 vTexOut;
out vec3 vMvPos;
out mat3 vNormalMat;


void main(void)
  {
  vTexOut = textureCoordinate;

  vNormalMat[1] = binormal;
  vNormalMat[2] = normal;
  vNormalMat[0] = cross(normal, binormal);

  vMvPos = (modelView * vec4(position, 1.0)).xyz;
  vPosition = position;
  }
