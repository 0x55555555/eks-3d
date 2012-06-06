attribute vec3 vertex;

uniform vec4 colour;
varying vec4 col;

void main(void)
  {
  col = colour;
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex,1.0);
  }
