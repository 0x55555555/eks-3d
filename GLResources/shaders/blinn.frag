uniform sampler2D ambientTexture;
varying vec2 textureCoordinate;
varying vec3 cameraSpaceNormalisedNormal;
varying vec4 cameraSpaceVertexPosition;

vec3 lighting(vec3 normal, vec4 pos, float specularity)
  {
  // diffuse
  vec3 lightDir = normalize(vec3(0.25, 1, 0.5));
  vec3 ambientLightColour = vec3(0.2, 0.2, 0.2);
  vec3 diffuseLightColour = vec3(1.0, 0.8, 0.78);
  vec3 specularLightColour = vec3(1.0, 0.8, 1.0);
  vec3 halfVector = normalize(vec3(0.0, 0.0, 1.0) + lightDir);
  float lightSpecularity = 100.0;

  vec3 diffuse = clamp(dot(lightDir, normal), 0.0, 1.0) * diffuseLightColour;

  vec3 specular = pow(clamp(dot(normal, halfVector), 0.0, 1.0), lightSpecularity) * vec3(specularity) * specularLightColour;

  return diffuse + specular + ambientLightColour;
  }

void main(void)
  {
  vec4 tex = texture2D(ambientTexture,textureCoordinate);
  gl_FragColor = vec4(tex.rgb*lighting(cameraSpaceNormalisedNormal, cameraSpaceVertexPosition, 30.0), tex.a);
  }
