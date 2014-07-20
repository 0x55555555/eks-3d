#pragma once
#include "../ExampleBase.h"
#include "XShader.h"
#include "XGeometry.h"
#include "XRasteriserState.h"
#include "XTransform.h"
#include "XTexture.h"
#include "XModeller.h"
#include "XCore.h"
#include "../geometry/geometry.h"
#include "QImage"
#include "QGLWidget"
#include "QPainter"
#include "QConicalGradient"

namespace Eks
{

namespace Demo
{

class TextureExample : public ExampleBase
  {
public:
  TextureExample()
    {
    _t = 0.0f;
    }

  void intialise(Renderer* r)
    {
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

    Eks::Texture2D::delayedCreate(_tex, r, im.width(), im.height(), Eks::Rgba8, im.constBits());

    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D rsc0;"
        "in vec2 texOut;"
        "in vec3 normOut;"
        "out vec4 outColour;"
        "void main(void)"
        "  {"
        "  outColour = (0.2 + max(0, dot(normalize(vec3(1,1,1)), normOut))) * texture(rsc0, texOut);"
        "  }";

    const char *vsrc =
        "layout (std140) uniform cb0 { mat4 model; mat4 modelView; mat4 modelViewProj; };"
        "layout (std140) uniform cb1 { mat4 view; mat4 proj; };"
        "in vec3 position;"
        "in vec3 normal;"
        "in vec2 textureCoordinate;"
        "out vec2 texOut;"
        "out vec3 normOut;"
        "void main(void)"
        "  {"
        "  texOut = textureCoordinate;"
        "  normOut = normal;"
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
    _shader.setShaderResource(0, &_tex);
    }

  void resize(Renderer*, xuint32 width, xuint32 height)
    {
    float aspect = (float)width / (float)height;

    _proj = TransformUtilities::perspective(Eks::degreesToRadians(45.0f), aspect, 0.1f, 100.0f);
    }

  void render(Renderer* r)
    {
    r->setProjectionTransform(_proj);

    _t += 0.005f;

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
  Texture2D _tex;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
