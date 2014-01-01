#ifndef XRENDERER_H
#define XRENDERER_H

#include "X3DGlobal.h"
#include "Utilities/XProperty.h"
#include "XTransform.h"

namespace Eks
{

class Renderer;
class ShaderConstantData;
class ShaderConstantDataDescription;
class Colour;
class Shader;
class Geometry;
class IndexGeometry;
class ShaderVertexLayout;
class ShaderVertexLayoutDescription;
class FrameBuffer;
class ScreenFrameBuffer;
class ShaderVertexComponent;
class ShaderFragmentComponent;
class Geometry;
class DepthStencilState;
class BlendState;
class RasteriserState;
class Texture2D;
class Resource;

class RendererStackTransform;

enum RendererShaderType
  {
  PlainColour,

  ShaderTypeCount
  };

enum RendererDebugLocatorMode
  {
  DebugLocatorBasic=0,
  DebugLocatorClearShader=1
  };

namespace detail
{

// creation for types
struct RendererCreateFunctions
  {
  bool (*framebuffer)(
      Renderer *r,
      FrameBuffer *b,
      xuint32 w,
      xuint32 h,
      xuint32 colourFormat,
      xuint32 depthFormat);

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

  bool (*texture2D)(
      Renderer *r,
      Texture2D *tex,
      xsize width,
      xsize height,
      xuint32 format,
      const void *data);

  bool (*shader)(
      Renderer *r,
      Shader *s,
      ShaderVertexComponent *v,
      ShaderFragmentComponent *f,
      const char **outputs,
      xsize outputCount);

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

  bool (*depthStencilState)(
      Renderer *r,
      DepthStencilState *s);

  bool (*blendState)(
      Renderer *r,
      BlendState *s);

  bool (*shaderConstantData)(
      Renderer *r,
      ShaderConstantData *,
      ShaderConstantDataDescription *desc,
      xsize descCount,
      const void *data);
  };

// destroy types
struct RendererDestroyFunctions
  {
  void (*framebuffer)(Renderer *r, FrameBuffer* s);
  void (*geometry)(Renderer *r, Geometry *);
  void (*indexGeometry)(Renderer *r, IndexGeometry *);
  void (*texture2D)(Renderer *r, Texture2D* s);
  void (*shader)(Renderer *r, Shader* s);
  void (*shaderVertexLayout)(Renderer *r, ShaderVertexLayout *d);
  void (*vertexShaderComponent)(Renderer *r, ShaderVertexComponent* s);
  void (*fragmentShaderComponent)(Renderer *r, ShaderFragmentComponent* s);
  void (*rasteriserState)(Renderer *r, RasteriserState *);
  void (*depthStencilState)(Renderer *r, DepthStencilState *);
  void (*blendState)(Renderer *r, BlendState *);
  void (*shaderConstantData)(Renderer *r, ShaderConstantData *);
  };

struct RendererSetFunctions
  {
  void (*clearColour)(Renderer *r, const Colour &col);

  void (*shaderConstantData)(Renderer *r, ShaderConstantData *, void *data);

  void (*viewTransform)(Renderer *r, const Transform &);
  void (*projectionTransform)(Renderer *r, const ComplexTransform &);

  void (*shaderConstantBuffer)(
    Renderer *r,
    Shader *shader,
    xsize index,
    xsize count,
    const ShaderConstantData * const* data);

  void (*shaderResource)(
    Renderer *r,
    Shader *shader,
    xsize index,
    xsize count,
    const Resource * const* data);

  // set the current shader
  void (*shader)(Renderer *r, const Shader *, const ShaderVertexLayout *layout);

  // set states
  void (*rasteriserState)(Renderer *r, const RasteriserState *state);
  void (*depthStencilState)(Renderer *r, const DepthStencilState *state);
  void (*blendState)(Renderer *r, const BlendState *state);

  void (*transform)(Renderer *r, const Transform &);

  void (*stockShader)(Renderer *r, RendererShaderType t, Shader *, const ShaderVertexLayout *);
  };

struct RendererGetFunctions
  {
  void (*texture2DInfo)(const Renderer *r, const Texture2D *tex, Eks::VectorUI2D& v);
  Shader *(*stockShader)(Renderer *r, RendererShaderType t, const ShaderVertexLayout **);
  };

struct RendererDrawFunctions
  {
  // draw the given geometry
  void (*indexedTriangles)(Renderer *r, const IndexGeometry *indices, const Geometry *vert);
  void (*triangles)(Renderer *r, const Geometry *vert);
  void (*indexedLines)(Renderer *r, const IndexGeometry *indices, const Geometry *vert);
  void (*lines)(Renderer *r, const Geometry *vert);
  void (*drawDebugLocator)(Renderer *r, RendererDebugLocatorMode);
  };

struct RendererFramebufferFunctions
  {
  void (*clear)(Renderer *r, FrameBuffer *buffer, xuint32 mode);
  bool (*resize)(Renderer *r, ScreenFrameBuffer *buffer, xuint32 w, xuint32 h, xuint32 rotation);
  void (*begin)(Renderer *r, FrameBuffer *buffer);
  void (*end)(Renderer *r, FrameBuffer *buffer);
  void (*present)(Renderer *r, ScreenFrameBuffer *buffer, bool *deviceLost);
  Texture2D *(*getTexture)(Renderer *r, FrameBuffer *buffer, xuint32 mode);
  };

struct RendererFunctions
  {
  RendererCreateFunctions create;
  RendererDestroyFunctions destroy;
  RendererSetFunctions set;
  RendererGetFunctions get;
  RendererDrawFunctions draw;
  RendererFramebufferFunctions frame;
  };

}

class EKS3D_EXPORT Renderer
  {
public:
  typedef RendererStackTransform StackTransform;

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

  void drawTriangles(const Geometry *g)
    {
    functions().draw.triangles(this, g);
    }

  void drawTriangles(const IndexGeometry *i, const Geometry *g)
    {
    functions().draw.indexedTriangles(this, i, g);
    }

  void drawLines(const Geometry *g)
    {
    functions().draw.lines(this, g);
    }

  void drawLines(const IndexGeometry *i, const Geometry *g)
    {
    functions().draw.indexedLines(this, i, g);
    }

  Shader *stockShader(RendererShaderType t, const ShaderVertexLayout **lay)
    {
    return functions().get.stockShader(this, t, lay);
    }

  void setStockShader(RendererShaderType t, Shader *s, const ShaderVertexLayout *lay)
    {
    return functions().set.stockShader(this, t, s, lay);
    }

XProperties:
  XRORefProperty(detail::RendererFunctions, functions);

protected:
  ~Renderer() { }
  XWriteProperty(detail::RendererFunctions, functions, setFunctions)
  };

}

#endif // XRENDERER_H
