attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texture;

varying vec2 textureCoordinate;
varying vec3 cameraSpaceNormalisedNormal;
varying vec4 cameraSpaceVertexPosition;

void main(void)
  {
  cameraSpaceNormalisedNormal = normalize((gl_ModelViewMatrix * vec4(normal,0.0)).xyz);
  textureCoordinate = texture;
  cameraSpaceVertexPosition = gl_ModelViewMatrix * vec4(vertex,1.0);
  gl_Position = gl_ProjectionMatrix * cameraSpaceVertexPosition;
  }
