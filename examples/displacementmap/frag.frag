#if X_GLSL_VERSION >= 130 || defined(X_GLES)
precision mediump float;
#endif

layout (std140) uniform cb0 { mat4 model; mat4 modelView; mat4 modelViewProj; };
layout (std140) uniform cb1 { mat4 view; mat4 proj; };

uniform sampler2D rsc0;
uniform sampler2D rsc1;
in vec2 teTexOut;
in vec3 teMvPos;
in mat3 teNormalMat;
out vec4 outColour;

void main(void)
  {
  vec3 lightPosition = (view * vec4(5, 10, 5, 1)).xyz;
  vec3 lightDir = (view * normalize(vec4(-1, -1, -1, 0))).xyz;
  float conAtt = 0;
  float linAtt = 0.05;
  float quadAtt = 0;

  float shininess = 6.0;

  vec4 ambient = vec4(0,0,0,0);
  vec4 diffuse = texture(rsc0, teTexOut);
  vec3 normal = teNormalMat * (normalize(texture(rsc1, teTexOut).xyz) - 0.5) * 2.0;
  vec4 specular = vec4(1, 1, 1, 0);
  vec4 colour = vec4(0.05, 0.05, 0.05, 0);

  float dist = length(teMvPos - lightPosition);

  float NdotL = max(0, dot(normal, normalize(-lightDir)));

  if (NdotL > 0.0)
    {
    float att = 1.0 / (conAtt + linAtt * dist + quadAtt * dist * dist);
    colour += att * (diffuse * NdotL + ambient);

    vec3 eye = normalize(-teMvPos);
    vec3 toLight = lightPosition - teMvPos;
    vec3 halfV = normalize(normalize(toLight) + eye).xyz;

    float NdotHV = max(dot(normal.xyz, halfV), 0.0);
    colour += att * specular * pow(NdotHV, shininess);
    }

  outColour = colour;
  return;
  }
