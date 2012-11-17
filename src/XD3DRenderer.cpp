#include <d3d11.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>
#include "XD3DRenderer.h"
#include "XD3DRendererImpl.h"
#include "XColour"
#include "XOptional"

using namespace DirectX;

XD3DRenderer::XD3DRenderer(IUnknown *window)
  {
  _impl = new XD3DRendererImpl;

  _impl->_window = window;
  _impl->createResources();
  setClearColour(XColour(0.0f, 0.0f, 0.0f));
  }

XD3DRenderer::~XD3DRenderer()
  {
  delete _impl;
  _impl = 0;
  }

ID3D11Device1 *XD3DRenderer::getD3DDevice()
  {
  return _impl->_d3dDevice.Get();
  }

ID3D11DeviceContext1 *XD3DRenderer::getD3DContext()
  {
  return _impl->_d3dContext.Get();
  }

void XD3DRenderer::beginFrame()
  {
  clear();
  _impl->setRenderTarget(&_impl->_renderTarget);
  }

void XD3DRenderer::endFrame(bool *deviceListOptional)
  {
  _impl->_renderTarget.present(_impl->_d3dContext.Get(), deviceListOptional);
  }

bool XD3DRenderer::resize(xuint32 w, xuint32 h, Rotation rotation)
  {
  return _impl->resize(w, h, rotation);
  }

void XD3DRenderer::pushTransform(const Transform & )
  {
  }

void XD3DRenderer::popTransform( )
  {
  }

void XD3DRenderer::setClearColour(const XColour &col)
  {
  _impl->_clearColour = col;
  }

void XD3DRenderer::clear(int clear)
  {
  _impl->_renderTarget.clear(
        _impl->_d3dContext.Get(),
        (clear&XRenderer::ClearColour) != 0,
        (clear&XRenderer::ClearDepth) != 0,
        _impl->_clearColour.data(),
        1.0f,
        0);
  }


XAbstractShader *XD3DRenderer::getShader()
  {
  return 0;
  }

XAbstractGeometry *XD3DRenderer::getGeometry( XBufferType )
  {
  return 0;
  }

XAbstractTexture *XD3DRenderer::getTexture()
  {
  return 0;
  }

XAbstractFramebuffer *XD3DRenderer::getFramebuffer(int,
                                     int,
                                     int,
                                     int,
                                     int)
  {
  xAssertFail();
  return 0;
  }

void XD3DRenderer::debugRenderLocator(DebugLocatorMode)
  {
  }


void XD3DRenderer::destroyShader( XAbstractShader * )
  {
  }

void XD3DRenderer::destroyGeometry( XAbstractGeometry * )
  {
  }

void XD3DRenderer::destroyTexture( XAbstractTexture * )
  {
  }

void XD3DRenderer::destroyFramebuffer( XAbstractFramebuffer * )
  {
  }

void XD3DRenderer::setProjectionTransform( const XComplexTransform & )
  {
  }

void XD3DRenderer::setShader( const XShader * )
  {
  }

void XD3DRenderer::drawGeometry( const XGeometry & )
  {
  }

void XD3DRenderer::setFramebuffer( const XFramebuffer * )
  {
  }

void XD3DRenderer::enableRenderFlag( RenderFlags )
  {
  }

void XD3DRenderer::disableRenderFlag( RenderFlags )
  {
  }
