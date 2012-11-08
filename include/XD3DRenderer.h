#ifndef XD3DRENDERER_H
#define XD3DRENDERER_H

#include "XRenderer.h"

struct IUnknown;
struct ID3D11Device;
struct ID3D11DeviceContext;

class XD3DRenderer;
class XD3DRendererImpl;

class XD3DRendererRenderFrame
  {
public:
  XD3DRendererRenderFrame(XD3DRenderer &r, bool *deviceLost);
  ~XD3DRendererRenderFrame();

private:
  XD3DRenderer &_renderer;
  bool *_deviceLost;
  };

class EKS3D_EXPORT XD3DRenderer : public XRenderer
  {
public:

  typedef enum Rotation
    {
    RotateNone,
    Rotate90,
    Rotate180,
    Rotate270,
    };

  XD3DRenderer(IUnknown *window, xuint32 w, xuint32 h, Rotation rotation);
  ~XD3DRenderer();

  typedef XD3DRendererRenderFrame RenderFrame;

  bool resize(xuint32 w, xuint32 h, Rotation rotation);

  ID3D11Device *getD3DDevice();
  ID3D11DeviceContext *getD3DContext();

  void beginFrame();
  void endFrame(bool *deviceLost);

  void pushTransform( const XTransform & ) X_OVERRIDE;
  void popTransform( ) X_OVERRIDE;

  void setClearColour(const XColour &col) X_OVERRIDE;
  void clear(int=ClearColour|ClearDepth) X_OVERRIDE;

  // creation accessors for abstract types
  XAbstractShader *getShader() X_OVERRIDE;
  XAbstractGeometry *getGeometry( XGeometry::BufferType ) X_OVERRIDE;
  XAbstractTexture *getTexture() X_OVERRIDE;
  XAbstractFramebuffer *getFramebuffer( int options, int colourFormat, int depthFormat, int width, int height ) X_OVERRIDE;

  void debugRenderLocator(DebugLocatorMode) X_OVERRIDE;

  // destroy abstract types
  void destroyShader( XAbstractShader * ) X_OVERRIDE;
  void destroyGeometry( XAbstractGeometry * ) X_OVERRIDE;
  void destroyTexture( XAbstractTexture * ) X_OVERRIDE;
  void destroyFramebuffer( XAbstractFramebuffer * ) X_OVERRIDE;

  void setProjectionTransform( const XComplexTransform & ) X_OVERRIDE;

  // set the current shader
  void setShader( const XShader * ) X_OVERRIDE;

  // draw the given geometry
  void drawGeometry( const XGeometry & ) X_OVERRIDE;

  // bind the given framebuffer for drawing
  void setFramebuffer( const XFramebuffer * ) X_OVERRIDE;

protected:
  void enableRenderFlag( RenderFlags ) X_OVERRIDE;
  void disableRenderFlag( RenderFlags ) X_OVERRIDE;

  XD3DRendererImpl *_impl;
  };

inline XD3DRendererRenderFrame::XD3DRendererRenderFrame(XD3DRenderer &r, bool *deviceLost)
  : _renderer(r), _deviceLost(deviceLost)
  {
  r.beginFrame();
  }

inline XD3DRendererRenderFrame::~XD3DRendererRenderFrame()
  {
  _renderer.endFrame(_deviceLost);
  }

#endif // XD3DRENDERER_H