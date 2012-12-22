#ifndef XRENDERER_H
#define XRENDERER_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XFlags"
#include "XTransform.h"

class XColour;
class XShape;
class XShader;
class XGeometry;
class XIndexGeometry;
class XShaderVertexLayout;
class XShaderVertexLayoutDescription;
class XFramebuffer;
class XShaderVertexComponent;
class XShaderFragmentComponent;
class XAbstractTexture;
class XAbstractFramebuffer;
class XGeometry;

enum XBufferType
  {
  Thing
  };

class XRendererType
  {
  XRefProperty(QAtomicInt, refCount);
  };

class EKS3D_EXPORT XRenderer
  {
public:

  typedef enum Rotation
    {
    RotateNone,
    Rotate90,
    Rotate180,
    Rotate270,
    };

  XRenderer( );
  virtual ~XRenderer( );

  typedef Eigen::Affine3f Transform;

  virtual void pushTransform( const Transform & ) = 0;
  virtual void popTransform( ) = 0;

  enum ClearMode
    {
    ClearColour = 1,
    ClearDepth = 2
    };
  virtual void setClearColour(const XColour &col) = 0;
  virtual void clear(int=ClearColour|ClearDepth) = 0;

  // creation accessors for abstract types
  virtual bool createGeometry(
      XGeometry *g,
      const void *data,
      xsize elementSize,
      xsize elementCount) = 0;

  virtual bool createIndexGeometry(
      XIndexGeometry *g,
      int type,
      const void *index,
      xsize indexCount) = 0;

  virtual bool createShader(
      XShader *s,
      XShaderVertexComponent *v,
      XShaderFragmentComponent *f) = 0;

  virtual bool createVertexShaderComponent(
      XShaderVertexComponent *v,
      const char *s,
      xsize l,
      const XShaderVertexLayoutDescription *vertexDescriptions,
      xsize vertexItemCount,
      XShaderVertexLayout *layout) = 0;

  virtual bool createFragmentShaderComponent(
      XShaderFragmentComponent *f,
      const char *s,
      xsize l) = 0;

  virtual XAbstractTexture *getTexture() = 0;
  virtual XAbstractFramebuffer *getFramebuffer( int options, int colourFormat, int depthFormat, int width, int height ) = 0;

  enum DebugLocatorMode
    {
    None=0,
    ClearShader=1
    };
  virtual void debugRenderLocator(DebugLocatorMode) = 0;

  // destroy abstract types
  virtual void destroyShader(XShader* s) = 0;
  virtual void destroyShaderVertexLayout(XShaderVertexLayout *d) = 0;
  virtual void destroyVertexShaderComponent(XShaderVertexComponent* s) = 0;
  virtual void destroyFragmentShaderComponent(XShaderFragmentComponent* s) = 0;
  virtual void destroyGeometry( XGeometry * ) = 0;
  virtual void destroyIndexGeometry( XIndexGeometry * ) = 0;
  virtual void destroyTexture( XAbstractTexture * ) = 0;
  virtual void destroyFramebuffer( XAbstractFramebuffer * ) = 0;

  enum RenderFlags { AlphaBlending=1, DepthTest=2, BackfaceCulling=4 };
  void setRenderFlags( int );
  virtual int renderFlags() const;

  virtual void setViewTransform( const XTransform & ) = 0;
  virtual void setProjectionTransform( const XComplexTransform & ) = 0;

  // set the current shader
  virtual void setShader( const XShader *, const XShaderVertexLayout *layout ) = 0;

  // draw the given geometry
  virtual void drawTriangles(const XIndexGeometry *indices, const XGeometry *vert) = 0;
  virtual void drawTriangles(const XGeometry *vert) = 0;

  // bind the given framebuffer for drawing
  virtual void setFramebuffer( const XFramebuffer * ) = 0;

protected:
  virtual void enableRenderFlag( RenderFlags ) = 0;
  virtual void disableRenderFlag( RenderFlags ) = 0;

private:
  XFlags<RenderFlags, int> _renderFlags;
  };

class XRendererFlagBlock
  {
public:
  XRendererFlagBlock(XRenderer *r, int flagsToSet) : _renderer(r)
    {
    _oldFlags = _renderer->renderFlags();
    _renderer->setRenderFlags(_oldFlags | flagsToSet);
    }

  ~XRendererFlagBlock()
    {
    _renderer->setRenderFlags(_oldFlags);
    }

private:
  XRenderer *_renderer;
  int _oldFlags;
  };

#endif // XRENDERER_H
