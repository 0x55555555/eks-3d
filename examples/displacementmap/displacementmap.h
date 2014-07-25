#pragma once
#include "../ExampleBase.h"
#include "XShader.h"
#include "XGeometry.h"
#include "XRasteriserState.h"
#include "XTransform.h"
#include "XTexture.h"
#include "../normalmap/normalmap.h"

namespace Eks
{

namespace Demo
{

class DisplacementMapExample : public ExampleBase
  {
public:
  DisplacementMapExample()
    {
    _t = 0.0f;
    }

  void initialise(Renderer* r)
    {
    NormalMapExample::initPlane(r, &_geo);

    QImage im = QGLWidget::convertToGLFormat(QImage(":/normalmap/diffuse.png"));
    Eks::Texture2D::delayedCreate(_diff, r, im.width(), im.height(), Eks::Rgba8, im.constBits());

    QImage im2 = QGLWidget::convertToGLFormat(QImage(":/normalmap/normals.png"));
    Eks::Texture2D::delayedCreate(_norm, r, im2.width(), im2.height(), Eks::Rgba8, im2.constBits());

    QImage im3 = QGLWidget::convertToGLFormat(QImage(":/normalmap/displacement.png"));
    Eks::Texture2D::delayedCreate(_disp, r, im3.width(), im3.height(), Eks::Rgba8, im3.constBits());

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

    auto f = readAll(":/displacementmap/frag.frag");
    auto v = readAll(":/displacementmap/vert.vert");
    auto tc = readAll(":/displacementmap/tess.tesscont");
    auto te = readAll(":/displacementmap/tess.tesseval");

    ShaderVertexComponent::delayedCreate(_v, r, v.constData(), v.size(), desc, X_ARRAY_COUNT(desc), &_layout);
    ShaderComponent::delayedCreate(_f, r, ShaderComponent::Fragment, f.constData(), f.size());
    ShaderComponent::delayedCreate(_tc, r, ShaderComponent::TesselationControl, tc.constData(), tc.size());
    ShaderComponent::delayedCreate(_te, r, ShaderComponent::TesselationEvaluator, te.constData(), te.size());

    ShaderComponent* comps[] = { &_v, &_f, &_tc, &_te };
    const char *outputs[] = { "outColour" };
    Shader::delayedCreate(_shader, r, comps, X_ARRAY_COUNT(comps), outputs, X_ARRAY_COUNT(outputs));
    _shader.setShaderResource(0, &_diff);
    _shader.setShaderResource(1, &_norm);
    _shader.setShaderResource(2, &_disp);
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
      Vector3D(sinf(_t) * 22.0f, cosf(0.4 + _t) * 4.0f + 6.0f, cosf(_t) * 22.0f),
      Vector3D(0, 0, 0),
      Vector3D(0, 1, 0));
    r->setViewTransform(l);

    r->setTransform(Transform::Identity());

    r->setShader(&_shader, &_layout);
    r->drawPatch(&_geo, 3);
    }

  float _t;
  ComplexTransform _proj;
  Geometry _geo;
  ShaderVertexLayout _layout;
  Shader _shader;
  ShaderComponent _te;
  ShaderComponent _tc;
  ShaderComponent _f;
  ShaderVertexComponent _v;
  Texture2D _diff;
  Texture2D _norm;
  Texture2D _disp;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
