varying vec3 normalData;
uniform vec4 diffuse;

void main(void)
  {
  float dist = gl_FragCoord.x + gl_FragCoord.y;
  bool aOrB = mod(dist, 20.0) < 10.0;
  if(aOrB)
    {
    gl_FragColor = diffuse;
    }
  else
    {
    gl_FragColor = vec4( abs(normalize(normalData)), 1.0 );
    }
  }
