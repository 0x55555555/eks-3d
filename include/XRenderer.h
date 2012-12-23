#ifndef XRENDERER_H
#define XRENDERER_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XFlags"
#include "XTransform.h"

namespace Eks
{

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

class EKS3D_EXPORT Renderer
  {
public:

  enum Rotation
    {
    RotateNone,
    Rotate90,
    Rotate180,
    Rotate270
    };

  Renderer( );
  virtual ~Renderer( );

  typedef Eigen::Affine3f Transform;

  virtual void pushTransform( const Transform & ) = 0;
  virtual void popTransform( ) = 0;

  enum ClearMode
    {
    ClearColour = 1,
    ClearDepth = 2
    };
  virtual void setClearColour(const Colour &col) = 0;
  virtual void clear(int=ClearColour|ClearDepth) = 0;

  // creation accessors for abstract types
  virtual bool createGeometry(
      Geometry *g,
      const void *data,
      xsize elementSize,
      xsize elementCount) = 0;

  virtual bool createIndexGeometry(
      IndexGeometry *g,
      int type,
      const void *index,
      xsize indexCount) = 0;

  virtual bool createShader(
      Shader *s,
      ShaderVertexComponent *v,
      ShaderFragmentComponent *f) = 0;

  virtual bool createVertexShaderComponent(
      ShaderVertexComponent *v,
      const char *s,
      xsize l,
      const ShaderVertexLayoutDescription *vertexDescriptions,
      xsize vertexItemCount,
      ShaderVertexLayout *layout) = 0;

  virtual bool createFragmentShaderComponent(
      ShaderFragmentComponent *f,
      const char *s,
      xsize l) = 0;

  enum DebugLocatorMode
    {
    None=0,
    ClearShader=1
    };
  virtual void debugRenderLocator(DebugLocatorMode) = 0;

  // destroy abstract types
  virtual void destroyShader(Shader* s) = 0;
  virtual void destroyShaderVertexLayout(ShaderVertexLayout *d) = 0;
  virtual void destroyVertexShaderComponent(ShaderVertexComponent* s) = 0;
  virtual void destroyFragmentShaderComponent(ShaderFragmentComponent* s) = 0;
  virtual void destroyGeometry(Geometry *) = 0;
  virtual void destroyIndexGeometry(IndexGeometry *) = 0;

  enum RenderFlags { AlphaBlending=1, DepthTest=2, BackfaceCulling=4 };
  void setRenderFlags( int );
  virtual int renderFlags() const;

  virtual void setViewTransform(const Transform &) = 0;
  virtual void setProjectionTransform(const ComplexTransform &) = 0;

  // set the current shader
  virtual void setShader(const Shader *, const ShaderVertexLayout *layout ) = 0;

  // draw the given geometry
  virtual void drawTriangles(const IndexGeometry *indices, const Geometry *vert) = 0;
  virtual void drawTriangles(const Geometry *vert) = 0;

  // bind the given framebuffer for drawing
  virtual void setFramebuffer(const Framebuffer *) = 0;

protected:
  virtual void enableRenderFlag( RenderFlags ) = 0;
  virtual void disableRenderFlag( RenderFlags ) = 0;

private:
  XFlags<RenderFlags, int> _renderFlags;
  };

class RendererFlagBlock
  {
public:
  RendererFlagBlock(Renderer *r, int flagsToSet) : _renderer(r)
    {
    _oldFlags = _renderer->renderFlags();
    _renderer->setRenderFlags(_oldFlags | flagsToSet);
    }

  ~RendererFlagBlock()
    {
    _renderer->setRenderFlags(_oldFlags);
    }

private:
  Renderer *_renderer;
  int _oldFlags;
  };

}

#endif // XRENDERER_H
