layout (std140) uniform cb0 { mat4 model; mat4 modelView; mat4 modelViewProj; };
layout (std140) uniform cb1 { mat4 view; mat4 proj; };
in vec3 position;
in vec3 normal;
in vec3 binormal;
in vec2 textureCoordinate;
out vec2 texOut;
out vec3 mvPos;
out mat3 normalMat;

void main(void)
  {
  texOut = textureCoordinate;

  normalMat[1] = binormal;
  normalMat[2] = normal;
  normalMat[0] = cross(normal, binormal);

  mvPos = (modelView * vec4(position, 1.0)).xyz;
  gl_Position = modelViewProj * vec4(position, 1.0);
  }
