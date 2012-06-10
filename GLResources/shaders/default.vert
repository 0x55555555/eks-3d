attribute vec3 vertex;
attribute vec3 normalData;
void main(void)
  {
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex,1.0);
  }
