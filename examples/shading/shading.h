#pragma once
#include "../ExampleBase.h"
#include "XShader.h"
#include "XGeometry.h"
#include "XRasteriserState.h"
#include "XTransform.h"
#include "XModeller.h"
#include "XBlendState.h"
#include "XDepthStencilState.h"
#include "XCore.h"


namespace Eks
{

namespace Demo
{

class ShadingExample : public ExampleBase
  {
public:
  ShadingExample()
    {
    _t = 0.0f;
    }

  static void buildSphere(Eks::Renderer *r, Eks::IndexGeometry *igeo, Eks::Geometry *geo)
    {
    Eks::Modeller m(Eks::Core::defaultAllocator());
    m.drawSphere(0.49, 75, 75);

    ShaderVertexLayoutDescription::Semantic semantics[] = {
      ShaderVertexLayoutDescription::Position,
      ShaderVertexLayoutDescription::Normal,
      ShaderVertexLayoutDescription::TextureCoordinate,
    };

    m.bakeTriangles(r, semantics, X_ARRAY_COUNT(semantics), igeo, geo);
    }

  void initialise(Renderer* r)
    {
    buildSphere(r, &_igeo, &_geo);

    const char *vsrc =
        "layout (std140) uniform cb0 { mat4 model; mat4 modelView; mat4 modelViewProj; };"
        "layout (std140) uniform cb1 { mat4 view; mat4 proj; };"
        "in vec3 position;"
        "in vec3 normal;"
        "out vec3 vNormal;"
        "out vec3 vPos;"
        "void main(void)"
        "  {"
        "  vNormal = normal;"
        "  vPos = (modelView * vec4(position, 1.0)).xyz;"
        "  gl_Position = modelViewProj * vec4(position, 1.0);"
        "  }";

    ShaderVertexLayoutDescription desc[] =
      {
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::Position,
        ShaderVertexLayoutDescription::FormatFloat3),
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::Normal,
        ShaderVertexLayoutDescription::FormatFloat3),
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::TextureCoordinate,
        ShaderVertexLayoutDescription::FormatFloat2),
      };

    ShaderVertexComponent::delayedCreate(_v, r, vsrc, strlen(vsrc), desc, X_ARRAY_COUNT(desc), &_layout);

    ShaderConstantDataDescription dataDesc[] = {
      { "ambient", ShaderConstantDataDescription::Float4 },
      { "albedo", ShaderConstantDataDescription::Float4 },
      { "specular", ShaderConstantDataDescription::Float4 },
      { "lightDir", ShaderConstantDataDescription::Float3 },
      { "shininess", ShaderConstantDataDescription::Float },
    };

    Eks::ShaderConstantData::delayedCreate(_dataGpu, r, dataDesc, X_ARRAY_COUNT(dataDesc), &_dataLocal);

    buildLambert(r, &_lambertF, &_lambert);
    buildPhong(r, &_blinnF, &_blinn);
    buildBlinn(r, &_phongF, &_phong);
    buildCookTorrance(r, &_cookTorranceF, &_cookTorrance);

    Eks::BlendState::delayedCreate(_defaultState, r, false);
    Eks::BlendState::delayedCreate(
      _additiveState,
      r,
      true,
      Eks::BlendState::Add,
      Eks::BlendState::One,
      Eks::BlendState::One,
      Eks::BlendState::Add,
      Eks::BlendState::One,
      Eks::BlendState::One);

    Eks::DepthStencilState::delayedCreate(
      _defaultDepthState,
      r,
      Eks::DepthStencilState::MaskAll,
      Eks::DepthStencilState::DepthTest);

    Eks::DepthStencilState::delayedCreate(
      _noDepthTestState,
      r,
      Eks::DepthStencilState::MaskAll,
      Eks::DepthStencilState::DepthTest,
      Eks::DepthStencilState::Equal);
    }

  void buildLambert(Eks::Renderer *r, Eks::ShaderComponent *sh, Eks::Shader *shader)
    {
    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "layout (std140) uniform cb2 {"
        "  vec4 ambient;"
        "  vec4 albedo;"
        "  vec4 specular;"
        "  vec3 lightDir;"
        "  float shininess;"
        "};"
        "in vec3 vNormal;"
        "out vec4 outColour;"
        "void main(void)"
        "  {"
        "  float diffuse = max(0, dot(vNormal, -lightDir));"
        "  outColour = vec4(ambient.rgb + (albedo.rgb * diffuse), 1.0);"
        "  }";

    ShaderComponent::delayedCreate(*sh, r, ShaderComponent::Fragment, fsrc, strlen(fsrc));

    ShaderComponent* comps[] = { &_v, sh };
    const char *outputs[] = { "outColour" };
    Shader::delayedCreate(*shader, r, comps, X_ARRAY_COUNT(comps), outputs, X_ARRAY_COUNT(outputs));

    shader->setShaderConstantData(0, &_dataGpu);
    }

  void buildPhong(Eks::Renderer *r, Eks::ShaderComponent *sh, Eks::Shader *shader)
    {
    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "layout (std140) uniform cb2 {"
        "  vec4 ambient;"
        "  vec4 albedo;"
        "  vec4 specular;"
        "  vec3 lightDir;"
        "  float shininess;"
        "};"
        "in vec3 vNormal;"
        "in vec3 vPos;"
        "out vec4 outColour;"
        "void main(void)"
        "  {"
        "  vec3 reflectDir = reflect(lightDir, vNormal);"
        "  vec3 viewDir = normalize(-vPos);"
        "  float lambertian = max(dot(-lightDir, vNormal), 0.0);"
        "  float specularComp = 0.0;"
        "  if(lambertian > 0.0)"
        "    {"
        "    float specAngle = max(dot(reflectDir, viewDir), 0.0);"
        "    specularComp = pow(specAngle, shininess);"
        "    }"
        "  outColour = vec4(ambient.rgb + lambertian*albedo.rgb + specularComp*specular.rgb, 1.0);"
        "  }";

    ShaderComponent::delayedCreate(*sh, r, ShaderComponent::Fragment, fsrc, strlen(fsrc));

    ShaderComponent* comps[] = { &_v, sh };
    const char *outputs[] = { "outColour" };
    Shader::delayedCreate(*shader, r, comps, X_ARRAY_COUNT(comps), outputs, X_ARRAY_COUNT(outputs));

    shader->setShaderConstantData(0, &_dataGpu);
    }

  void buildBlinn(Eks::Renderer *r, Eks::ShaderComponent *sh, Eks::Shader *shader)
    {
    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "layout (std140) uniform cb2 {"
        "  vec4 ambient;"
        "  vec4 albedo;"
        "  vec4 specular;"
        "  vec3 lightDir;"
        "  float shininess;"
        "};"
        "in vec3 vNormal;"
        "in vec3 vPos;"
        "out vec4 outColour;"
        "void main(void)"
        "  {"
        "  float lambertian = max(dot(-lightDir, vNormal), 0.0);"
        "  float specularComp = 0.0;"
        "  if(lambertian > 0.0)"
        "    {"
        "    vec3 viewDir = normalize(-vPos);"
        "    vec3 halfDir = normalize(-lightDir + viewDir);"
        "    float specAngle = max(dot(halfDir, vNormal), 0.0);"
        "    specularComp = pow(specAngle, shininess);"
        "    }"
        "  outColour = vec4(ambient.rgb + lambertian * albedo.rgb + specularComp * specular.rgb, 1.0);"
        "  }";

    ShaderComponent::delayedCreate(*sh, r, ShaderComponent::Fragment, fsrc, strlen(fsrc));

    ShaderComponent* comps[] = { &_v, sh };
    const char *outputs[] = { "outColour" };
    Shader::delayedCreate(*shader, r, comps, X_ARRAY_COUNT(comps), outputs, X_ARRAY_COUNT(outputs));

    shader->setShaderConstantData(0, &_dataGpu);
    }

  void buildCookTorrance(Eks::Renderer *r, Eks::ShaderComponent *sh, Eks::Shader *shader)
    {
    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "layout (std140) uniform cb2 {"
        "  vec4 ambient;"
        "  vec4 albedo;"
        "  vec4 specular;"
        "  vec3 lightDir;"
        "  float shininess;"
        "};"
        "in vec3 vNormal;"
        "in vec3 vPos;"
        "out vec4 outColour;"
        "void main(void)"
        "  {"
        "  float roughnessValue = shininess;"
        "  float F0 = 0.8;"
        "  float k = 0.5;"
        "  vec3 lightColor = vec3(0.9, 0.1, 0.1);"
        "  float NdotL = max(dot(vNormal, -lightDir), 0.0);"
        "  float specularComp = 0.0;"
        "  if (NdotL > 0.0)"
        "    {"
        "    vec3 eyeDir = normalize(-vPos);"
        "    vec3 halfVector = normalize(-lightDir + eyeDir);"
        "    float NdotH = max(dot(vNormal, halfVector), 0.0);"
        "    float NdotV = max(dot(vNormal, eyeDir), 0.0);"
        "    float VdotH = max(dot(eyeDir, halfVector), 0.0);"
        "    float mSquared = roughnessValue * roughnessValue;"
        "    float NH2 = 2.0 * NdotH;"
        "    float g1 = (NH2 * NdotV) / VdotH;"
        "    float g2 = (NH2 * NdotL) / VdotH;"
        "    float geoAtt = min(1.0, min(g1, g2));"
        "    float r1 = 1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));"
        "    float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);"
        "    float roughness = r1 * exp(r2);"
        "    float fresnel = pow(1.0 - VdotH, 5.0);"
        "    fresnel *= (1.0 - F0);"
        "    fresnel += F0;"
        "    specularComp = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);"
        "    }"
        "  vec3 finalValue = ambient.rgb + (albedo.rgb * NdotL * k) + (specular.rgb * NdotL * (specularComp * (1.0 - k)));"
        "  outColour = vec4(finalValue, 1.0);"
        "  }";

    ShaderComponent::delayedCreate(*sh, r, ShaderComponent::Fragment, fsrc, strlen(fsrc));

    ShaderComponent* comps[] = { &_v, sh };
    const char *outputs[] = { "outColour" };
    Shader::delayedCreate(*shader, r, comps, X_ARRAY_COUNT(comps), outputs, X_ARRAY_COUNT(outputs));

    shader->setShaderConstantData(0, &_dataGpu);
    }

  void resize(Renderer*, xuint32 width, xuint32 height)
    {
    float aspect = (float)width / (float)height;

    _proj = TransformUtilities::perspective(Eks::degreesToRadians(35.0f), aspect, 0.1f, 100.0f);
    }

  void render(Renderer* r, Eks::FrameBuffer *buffer)
    {
    Eks::FrameBufferRenderFrame fr(r, buffer);

    r->setProjectionTransform(_proj);

    _t += 0.004f;

    const float t = 0.3f;
    Transform l = TransformUtilities::lookAt(
      Vector3D(sinf(t) * 3.2f, 1.0f, cosf(t) * 3.2f),
      Vector3D(0, 0.5, 0),
      Vector3D(0, 1, 0));
    r->setViewTransform(l);

    auto renderSphere = [r, this](const Eks::Vector3D &v, Shader *s, float shininess)
      {
      Eks::Transform tx = Eks::Transform::Identity();
      tx.translation() = v;

      _dataLocal.shininess = shininess;
      _dataGpu.update(&_dataLocal);

      r->setTransform(tx);
      r->setShader(s, &_layout);
      r->drawTriangles(&_igeo, &_geo);
      };

    Eks::Vector4D surfaceAlbedo = Eks::Vector4D(1, 1, 1, 1);
    Eks::Vector4D surfaceSpecular = Eks::Vector4D(1, 1, 1, 1);

    struct Light
      {
      Eks::Vector4D ambientLight;
      Eks::Vector4D colour;
      Eks::Vector3D direction;
      };

    Light lightPasses[] = {
      // Ambient
      { Eks::Vector4D(0.2, 0.2, 0.2 ,1) , Eks::Vector4D::Zero(), Eks::Vector3D::Zero() },

      // Light 1
      {
        Eks::Vector4D::Zero(),
        Eks::Vector4D(1, 1, 1, 1),
        Vector3D(sinf(_t) * 3.0f, -1.0f, cosf(_t) * -3.0f).normalized()
      },

      // Light 2
      {
        Eks::Vector4D::Zero(),
        Eks::Vector4D(0.6, 0.5, 0.4, 1),
        Vector3D(cosf(_t) * 3.0f, -1.0f, sinf(_t) * -3.0f).normalized()
      },

      // Light 3
      {
        Eks::Vector4D::Zero(),
        Eks::Vector4D(0.3, 0.6, 0.4, 1),
        Vector3D(cosf(_t) * 3.0f, sinf(_t) * -3.0f, 0.0f).normalized()
      },
    };

    for (xsize i = 0; i < X_ARRAY_COUNT(lightPasses); ++i)
      {
      r->setBlendState(i == 0 ? &_defaultState : &_additiveState);
      r->setDepthStencilState(i == 0 ? &_defaultDepthState : &_noDepthTestState);
      _dataLocal.ambient = lightPasses[i].ambientLight;
      _dataLocal.lightDir = lightPasses[i].direction;

      _dataLocal.albedo = lightPasses[i].colour.cwiseProduct(surfaceAlbedo);
      _dataLocal.specular = lightPasses[i].colour.cwiseProduct(surfaceSpecular);

      renderSphere(Eks::Vector3D(-1, 2, 0), &_lambert, 1);

      renderSphere(Eks::Vector3D(-1, 1, 0), &_phong, 1);
      renderSphere(Eks::Vector3D( 0, 1, 0), &_phong, 4);
      renderSphere(Eks::Vector3D( 1, 1, 0), &_phong, 16);

      renderSphere(Eks::Vector3D(-1, 0, 0), &_blinn, 1);
      renderSphere(Eks::Vector3D( 0, 0, 0), &_blinn, 4);
      renderSphere(Eks::Vector3D( 1, 0, 0), &_blinn, 16);

      renderSphere(Eks::Vector3D(-1, -1, 0), &_cookTorrance, 1);
      renderSphere(Eks::Vector3D( 0, -1, 0), &_cookTorrance, 0.3);
      renderSphere(Eks::Vector3D( 1, -1, 0), &_cookTorrance, 0.05);
      }
    }

  float _t;
  ComplexTransform _proj;
  Geometry _geo;
  IndexGeometry _igeo;
  ShaderVertexLayout _layout;
  ShaderVertexComponent _v;

  struct
    {
    Eks::Vector4D ambient;
    Eks::Vector4D albedo;
    Eks::Vector4D specular;

    Eks::Vector3DTight lightDir;
    float shininess;
    } _dataLocal;
  ShaderConstantData _dataGpu;

  Shader _lambert;
  ShaderComponent _lambertF;

  Shader _phong;
  ShaderComponent _phongF;

  Shader _blinn;
  ShaderComponent _blinnF;

  Shader _cookTorrance;
  ShaderComponent _cookTorranceF;

  BlendState _defaultState;
  BlendState _additiveState;

  DepthStencilState _defaultDepthState;
  DepthStencilState _noDepthTestState;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
