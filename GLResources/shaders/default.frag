varying vec3 normal;

void main(void)
  {
  float dist = gl_FragCoord.x + gl_FragCoord.y;
  bool aOrB = mod(dist, 20.0) < 10.0;
  if(aOrB)
    {
    gl_FragColor = vec4( 0.2, 0.2, 0.2, 1.0 );
    }
  else
    {
    gl_FragColor = vec4( abs(normalize(normal)), 1.0 );
    }
  }
