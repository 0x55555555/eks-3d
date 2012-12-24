#ifndef XRENDERER_H
#define XRENDERER_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XTransform.h"

namespace Eks
{

class Renderer;
class ShaderConstantData;
class Colour;
class Shader;
class Geometry;
class IndexGeometry;
class ShaderVertexLayout;
class ShaderVertexLayoutDescription;
class Framebuffer;
class ShaderVertexComponent;
class ShaderFragmentComponent;
class Geometry;
class RasteriserState;

class RendererStackTransform;

enum RendererDebugLocatorMode
  {
  DebugLocatorBasic=0,
  DebugLocatorClearShader=1
  };

enum RendererRotation
  {
  RotateNone,
  Rotate90,
  Rotate180,
  Rotate270
  };

enum RendererClearMode
  {
  ClearColour = 1,
  ClearDepth = 2
  };

namespace detail
{

// creation for types
struct RendererCreateFunctions
  {
  bool (*geometry)(
      Renderer *r,
      Geometry *g,
      const void *data,
      xsize elementSize,
      xsize elementCount);

  bool (*indexGeometry)(
      Renderer *r,
      IndexGeometry *g,
      int type,
      const void *index,
      xsize indexCount);

  bool (*shader)(
      Renderer *r,
      Shader *s,
      ShaderVertexComponent *v,
      ShaderFragmentComponent *f);

  bool (*vertexShaderComponent)(
      Renderer *r,
      ShaderVertexComponent *v,
      const char *s,
      xsize l,
      const ShaderVertexLayoutDescription *vertexDescriptions,
      xsize vertexItemCount,
      ShaderVertexLayout *layout);

  bool (*fragmentShaderComponent)(
      Renderer *r,
      ShaderFragmentComponent *f,
      const char *s,
      xsize l);

  bool (*rasteriserState)(
      Renderer *r,
      RasteriserState *s,
      xuint32 cull);

  bool (*shaderConstantData)(
      Renderer *r,
      ShaderConstantData *,
      xsize size,
      void *data);
  };

// destroy types
struct RendererDestroyFunctions
  {
  void (*geometry)(Renderer *r, Geometry *);
  void (*indexGeometry)(Renderer *r, IndexGeometry *);
  void (*shader)(Renderer *r, Shader* s);
  void (*shaderVertexLayout)(Renderer *r, ShaderVertexLayout *d);
  void (*vertexShaderComponent)(Renderer *r, ShaderVertexComponent* s);
  void (*fragmentShaderComponent)(Renderer *r, ShaderFragmentComponent* s);
  void (*rasteriserState)(Renderer *r, RasteriserState *);
  void (*shaderConstantData)(Renderer *r, ShaderConstantData *);
  };

struct RendererSetFunctions
  {
  void (*clearColour)(Renderer *r, const Colour &col);

  void (*shaderConstantData)(Renderer *r, ShaderConstantData *, void *data);

  void (*viewTransform)(Renderer *r, const Transform &);
  void (*projectionTransform)(Renderer *r, const ComplexTransform &);

  void (*fragmentShaderConstantBuffer)(
    Renderer *r,
    Shader *shader,
    xsize index,
    const ShaderConstantData *data);

  void (*vertexShaderConstantBuffer)(
    Renderer *r,
    Shader *shader,
    xsize index,
    const ShaderConstantData *data);

  // set the current shader
  void (*shader)(Renderer *r, const Shader *, const ShaderVertexLayout *layout);

  // set rasteriser
  void (*rasteriserState)(Renderer *r, const RasteriserState *state);

  // bind the given framebuffer for drawing
  void (*framebuffer)(Renderer *r, const Framebuffer *);

  void (*transform)(Renderer *r, const Transform &);
  };

struct RendererDrawFunctions
  {
  // draw the given geometry
  void (*indexedTriangles)(Renderer *r, const IndexGeometry *indices, const Geometry *vert);
  void (*triangles)(Renderer *r, const Geometry *vert);
  };

struct RendererFunctions
  {
  RendererCreateFunctions create;
  RendererDestroyFunctions destroy;
  RendererSetFunctions set;
  RendererDrawFunctions draw;

  void (*clear)(Renderer *r, int);
  bool (*resize)(Renderer *r, xuint32 w, xuint32 h, RendererRotation rotation);
  void (*beginFrame)(Renderer *r);
  void (*endFrame)(Renderer *r, bool *deviceLost);

  void (*debugRenderLocator)(Renderer *r, RendererDebugLocatorMode);
  };

}

class RendererRenderFrame
  {
public:
  RendererRenderFrame(Renderer *r, bool *deviceLost);
  ~RendererRenderFrame();

private:
  Renderer *_renderer;
  bool *_deviceLost;
  };

class EKS3D_EXPORT Renderer
  {
public:
  typedef RendererStackTransform StackTransform;
  typedef RendererRenderFrame RenderFrame;

  void setProjectionTransform(const ComplexTransform &tr)
    {
    functions().set.projectionTransform(this, tr);
    }

  void setViewTransform(const Transform &tr)
    {
    functions().set.viewTransform(this, tr);
    }

  void setTransform(const Transform &tr)
    {
    functions().set.transform(this, tr);
    }

  void setClearColour(const Colour &c)
    {
    functions().set.clearColour(this, c);
    }

  void setShader(const Shader *s, const ShaderVertexLayout *layout)
    {
    functions().set.shader(this, s, layout);
    }

  void setRasteriserState(const RasteriserState *s)
    {
    functions().set.rasteriserState(this, s);
    }

  void drawTriangles(Geometry *g)
    {
    functions().draw.triangles(this, g);
    }

  void drawTriangles(IndexGeometry *i, Geometry *g)
    {
    functions().draw.indexedTriangles(this, i, g);
    }

  void resize(xuint32 w, xuint32 h, RendererRotation rotation)
    {
    functions().resize(this, w, h, rotation);
    }

XProperties:
  XRORefProperty(detail::RendererFunctions, functions);

protected:
  ~Renderer() { }
  XWriteProperty(detail::RendererFunctions, functions, setFunctions)
  };


inline RendererRenderFrame::RendererRenderFrame(Renderer *r, bool *deviceLost)
    : _renderer(r), _deviceLost(deviceLost)
  {
  _renderer->functions().beginFrame(_renderer);
  }

inline RendererRenderFrame::~RendererRenderFrame()
  {
  _renderer->functions().endFrame(_renderer, _deviceLost);
  }

}

#endif // XRENDERER_H
