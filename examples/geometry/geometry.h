#pragma once
#include "../ExampleBase.h"
#include "XShader.h"
#include "XGeometry.h"
#include "XRasteriserState.h"
#include "XTransform.h"
#include "XModeller.h"
#include "XCore.h"


namespace Eks
{

namespace Demo
{

class GeometryExample : public ExampleBase
  {
public:
  GeometryExample()
    {
    _t = 0.0f;
    }

  static void buildGeometry(Eks::Renderer *r, Eks::IndexGeometry *igeo, Eks::Geometry *geo)
    {
    Eks::Modeller m(Eks::Core::defaultAllocator());

    Eks::Transform tx = Eks::Transform::Identity();
    tx.translate(Eks::Vector3D(1, 1, 1));

    m.drawCube();
    m.setTransform(tx);
    m.drawSphere(1.1);

    ShaderVertexLayoutDescription::Semantic semantics[] = {
      ShaderVertexLayoutDescription::Position,
      ShaderVertexLayoutDescription::Normal,
      ShaderVertexLayoutDescription::TextureCoordinate,
    };

    m.bakeTriangles(r, semantics, X_ARRAY_COUNT(semantics), igeo, geo);
    }

  void initialise(Renderer* r)
    {
    buildGeometry(r, &_igeo, &_geo);

    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "in vec3 colOut;"
        "out vec4 outColour;"
        "void main(void)"
        "  {"
        "  outColour = vec4(abs(colOut), 1.0);"
        "  }";

    const char *vsrc =
        "layout (std140) uniform cb0 { mat4 model; mat4 modelView; mat4 modelViewProj; };"
        "layout (std140) uniform cb1 { mat4 view; mat4 proj; };"
        "in vec3 position;"
        "in vec2 textureCoordinate;"
        "out vec3 colOut;"
        "void main(void)"
        "  {"
        "  colOut = vec3(textureCoordinate, 0);"
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
    ShaderComponent::delayedCreate(_f, r, ShaderComponent::Fragment, fsrc, strlen(fsrc));

    ShaderComponent* comps[] = { &_v, &_f };
    const char *outputs[] = { "outColour" };
    Shader::delayedCreate(_shader, r, comps, X_ARRAY_COUNT(comps), outputs, X_ARRAY_COUNT(outputs));
    }

  void resize(Renderer*, xuint32 width, xuint32 height)
    {
    float aspect = (float)width / (float)height;

    _proj = TransformUtilities::perspective(Eks::degreesToRadians(45.0f), aspect, 0.1f, 100.0f);
    }

  void render(Renderer* r, Eks::FrameBuffer *buffer)
    {
    Eks::FrameBufferRenderFrame fr(r, buffer);

    r->setProjectionTransform(_proj);

    _t += 0.002f;

    Transform l = TransformUtilities::lookAt(
      Vector3D(sinf(_t) * 3.0f, 1.5f, cosf(_t) * 3.0f),
      Vector3D(0, 1, 0),
      Vector3D(0, 1, 0));
    r->setViewTransform(l);

    r->setTransform(Transform::Identity());

    r->setShader(&_shader, &_layout);
    r->drawTriangles(&_igeo, &_geo);
    }

  float _t;
  ComplexTransform _proj;
  Geometry _geo;
  IndexGeometry _igeo;
  ShaderVertexLayout _layout;
  Shader _shader;
  ShaderComponent _f;
  ShaderVertexComponent _v;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
