#pragma once
#include "../ExampleBase.h"
#include "XShader.h"
#include "XGeometry.h"
#include "XRasteriserState.h"
#include "XTransform.h"
#include "XTexture.h"

namespace Eks
{

namespace Demo
{

class NormalMapExample : public ExampleBase
  {
public:
  NormalMapExample()
    {
    _t = 0.0f;
    }

  static void initPlane(Renderer* r, Geometry *geo)
    {
    const float vert[] = {
      -10, 0, -10,
      0, 0,
      0, 1, 0,
      1, 0, 0,
      10, 0, 10,
      1, 1,
      0, 1, 0,
      1, 0, 0,
      10, 0, -10,
      1, 0,
      0, 1, 0,
      1, 0, 0,
      10, 0, 10,
      1, 1,
      0, 1, 0,
      1, 0, 0,
      -10, 0, 10,
      0, 1,
      0, 1, 0,
      1, 0, 0,
      -10, 0, -10,
      0, 0,
      0, 1, 0,
      1, 0, 0,
    };
    const xsize vertCount = X_ARRAY_COUNT(vert) / 11;
    Geometry::delayedCreate(*geo, r, vert, sizeof(float) * 11, vertCount);
    }

  void initialise(Renderer* r)
    {
    initPlane(r, &_geo);

    QImage im = QGLWidget::convertToGLFormat(QImage(":/normalmap/diffuse.png"));
    Eks::Texture2D::delayedCreate(_diff, r, im.width(), im.height(), Eks::Rgba8, im.constBits());

    QImage im2 = QGLWidget::convertToGLFormat(QImage(":/normalmap/normals.png"));
    Eks::Texture2D::delayedCreate(_norm, r, im2.width(), im2.height(), Eks::Rgba8, im2.constBits());

    ShaderVertexLayoutDescription desc[] =
      {
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::Position,
        ShaderVertexLayoutDescription::FormatFloat3),
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::TextureCoordinate,
        ShaderVertexLayoutDescription::FormatFloat2),
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::Normal,
        ShaderVertexLayoutDescription::FormatFloat3),
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::BiNormal,
        ShaderVertexLayoutDescription::FormatFloat3),
      };

    auto readAll = [](QString str) -> QByteArray
      {
      QFile f(str);
      xVerify(f.open(QFile::ReadOnly));
      return f.readAll();
      };

    auto f = readAll(":/normalmap/frag.frag");
    auto v = readAll(":/normalmap/vert.vert");

    ShaderVertexComponent::delayedCreate(_v, r, v.constData(), v.size(), desc, X_ARRAY_COUNT(desc), &_layout);
    ShaderComponent::delayedCreate(_f, r, ShaderComponent::Fragment, f.constData(), f.size());

    ShaderComponent* comps[] = { &_v, &_f };
    const char *outputs[] = { "outColour" };
    Shader::delayedCreate(_shader, r, comps, X_ARRAY_COUNT(comps), outputs, X_ARRAY_COUNT(outputs));
    _shader.setShaderResource(0, &_diff);
    _shader.setShaderResource(1, &_norm);
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

    _t += 0.005f;

    Transform l = TransformUtilities::lookAt(
      Vector3D(sinf(_t) * 22.0f, 10, cosf(_t) * 22.0f),
      Vector3D(0, 0, 0),
      Vector3D(0, 1, 0));
    r->setViewTransform(l);

    r->setTransform(Transform::Identity());

    r->setShader(&_shader, &_layout);
    r->drawTriangles(&_geo);
    }

  float _t;
  ComplexTransform _proj;
  Geometry _geo;
  ShaderVertexLayout _layout;
  Shader _shader;
  ShaderComponent _f;
  ShaderVertexComponent _v;
  Texture2D _diff;
  Texture2D _norm;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
