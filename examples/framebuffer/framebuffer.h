#pragma once
#include "../ExampleBase.h"
#include "XShader.h"
#include "XGeometry.h"
#include "XRasteriserState.h"
#include "XTransform.h"
#include "XModeller.h"
#include "XFramebuffer.h"
#include "XCore.h"
#include "../geometry/geometry.h"


namespace Eks
{

namespace Demo
{

class FramebufferExample : public ExampleBase
  {
public:
  FramebufferExample()
    {
    _t = 0.0f;
    }

  void initialise(Renderer* r)
    {
    _geom.initialise(r);

    GeometryExample::buildGeometry(r, &_igeo, &_geo);

    QImage im(512, 512, QImage::Format_ARGB32_Premultiplied);
    {
      QPainter p(&im);

      QConicalGradient grad(QPointF(256, 256), 128);
      grad.setColorAt(0, Qt::red);
      grad.setColorAt(1, Qt::green);
      p.fillRect(0, 0, 512, 512, grad);

      QBrush pattern(Qt::DiagCrossPattern);
      p.fillRect(0, 0, 512, 512, pattern);
    }
    im = QGLWidget::convertToGLFormat(im);

    Eks::FrameBuffer::delayedCreate(_fb, r, 512, 512, Eks::Rgba8, Eks::Depth24);

    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D rsc0;"
        "in vec2 texOut;"
        "out vec4 outColour;"
        "void main(void)"
        "  {"
        "  outColour = clamp(vec4(0.2) + 0.8 * texture(rsc0, mod(texOut * 10, 1)), 0, 1);"
        "  }";

    const char *vsrc =
        "layout (std140) uniform cb0 { mat4 model; mat4 modelView; mat4 modelViewProj; };"
        "layout (std140) uniform cb1 { mat4 view; mat4 proj; };"
        "in vec3 position;"
        "in vec2 textureCoordinate;"
        "out vec2 texOut;"
        "void main(void)"
        "  {"
        "  texOut = textureCoordinate;"
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
    _shader.setShaderResource(0, _fb.getTexture(Eks::FrameBuffer::TextureColour));
    }

  void resize(Renderer *r, xuint32 width, xuint32 height)
    {
    _geom.resize(r, width, height);

    float aspect = (float)width / (float)height;

    _proj = TransformUtilities::perspective(Eks::degreesToRadians(45.0f), aspect, 0.1f, 100.0f);
    }

  void render(Renderer* r, Eks::FrameBuffer *buffer)
    {
    // render the geom example
    _geom.render(r, &_fb);

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
  FrameBuffer _fb;
  GeometryExample _geom;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
