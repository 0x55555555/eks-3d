attribute vec3 vertex;
attribute vec3 normals;
varying vec3 normal;
void main(void)
  {
  normal = normals;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex,1.0);
  }
