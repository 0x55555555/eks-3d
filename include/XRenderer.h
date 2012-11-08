#ifndef XRENDERER_H
#define XRENDERER_H

#include "X3DGlobal.h"
#include "XGeometry.h"
#include "XProperty"
#include "XFlags"

class XColour;
class XShape;
typedef XList<XShape> XShapeList;
class XAbstractGeometry;
class XShader;
class XFramebuffer;
class XAbstractShader;
class XAbstractTexture;
class XAbstractFramebuffer;

class XRendererType
  {
  XRefProperty(QAtomicInt, refCount);
  };

class EKS3D_EXPORT XRenderer
    {
public:
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
    virtual XAbstractShader *getShader( ) = 0;
    virtual XAbstractGeometry *getGeometry( XGeometry::BufferType ) = 0;
    virtual XAbstractTexture *getTexture() = 0;
    virtual XAbstractFramebuffer *getFramebuffer( int options, int colourFormat, int depthFormat, int width, int height ) = 0;

    enum DebugLocatorMode
      {
      None=0,
      ClearShader=1
      };
    virtual void debugRenderLocator(DebugLocatorMode) = 0;

    // destroy abstract types
    virtual void destroyShader( XAbstractShader * ) = 0;
    virtual void destroyGeometry( XAbstractGeometry * ) = 0;
    virtual void destroyTexture( XAbstractTexture * ) = 0;
    virtual void destroyFramebuffer( XAbstractFramebuffer * ) = 0;

    enum RenderFlags { AlphaBlending=1, DepthTest=2, BackfaceCulling=4 };
    void setRenderFlags( int );
    virtual int renderFlags() const;

    virtual void setProjectionTransform( const XComplexTransform & ) = 0;

    // set the current shader
    virtual void setShader( const XShader * ) = 0;

    // draw the given geometry
    virtual void drawGeometry( const XGeometry & ) = 0;

    // bind the given framebuffer for drawing
    virtual void setFramebuffer( const XFramebuffer * ) = 0;

    void drawShape( XShape & );
    void drawShapes( XShapeList & );

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
