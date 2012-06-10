attribute vec3 vertex;
attribute vec3 normalData;
varying vec3 normal;

void main(void)
  {
  normal = normalData;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex,1.0);
  }
