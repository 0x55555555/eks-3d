#include <d3d11.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>
#include "XD3DRenderer.h"
#include "XD3DRendererImpl.h"
#include "XColour"
#include "XOptional"

using namespace DirectX;

XD3DRenderer::XD3DRenderer(IUnknown *window, xuint32 w, xuint32 h, Rotation rotation)
  {
  _impl = new XD3DRendererImpl;

  _impl->_window = window;
  _impl->createResources();
  resize(w, h, rotation);
  setClearColour(XColour(0.0f, 0.0f, 0.0f));
  }

XD3DRenderer::~XD3DRenderer()
  {
  delete _impl;
  _impl = 0;
  }

ID3D11Device *XD3DRenderer::getD3DDevice()
  {
  return _impl->_d3dDevice;
  }

ID3D11DeviceContext *XD3DRenderer::getD3DContext()
  {
  return _impl->_d3dContext;
  }

void XD3DRenderer::beginFrame()
  {
  clear();
  _impl->_d3dContext->OMSetRenderTargets(
    1,
    &_impl->_renderTargetView,
    _impl->_depthStencilView
    );
  }

void XD3DRenderer::endFrame(bool *deviceListOptional)
  {
  XOptional<bool> deviceLost(deviceListOptional);
  deviceLost = true;

  // The application may optionally specify "dirty" or "scroll"
  // rects to improve efficiency in certain scenarios.
  DXGI_PRESENT_PARAMETERS parameters = {0};
  parameters.DirtyRectsCount = 0;
  parameters.pDirtyRects = nullptr;
  parameters.pScrollRect = nullptr;
  parameters.pScrollOffset = nullptr;

  // The first argument instructs DXGI to block until VSync, putting the application
  // to sleep until the next VSync. This ensures we don't waste any cycles rendering
  // frames that will never be displayed to the screen.
  HRESULT hr = _impl->_swapChain->Present1(1, 0, &parameters);

  // Discard the contents of the render target.
  // This is a valid operation only when the existing contents will be entirely
  // overwritten. If dirty or scroll rects are used, this call should be removed.
  _impl->_d3dContext->DiscardView(_impl->_renderTargetView);

  // Discard the contents of the depth stencil.
  _impl->_d3dContext->DiscardView(_impl->_depthStencilView);

  // If the device was removed either by a disconnect or a driver upgrade, we
  // must recreate all device resources.
  if (hr == DXGI_ERROR_DEVICE_REMOVED)
    {
    deviceLost = false;
    }
  else
    {
    if(FAILED(hr))
      {
      xAssertFail();
      }
    }
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
  XColour col2;
  Eigen::internal::lazyAssign(col2, col);
  //_impl->_clearColour = col;
  }

void XD3DRenderer::clear(int clear)
  {
  if((clear&ClearColour) != 0)
    {
    _impl->_d3dContext->ClearRenderTargetView(
      _impl->_renderTargetView,
      _impl->_clearColour.data()
      );
    }

  if((clear&ClearDepth) != 0)
    {
    _impl->_d3dContext->ClearDepthStencilView(
      _impl->_depthStencilView,
      D3D11_CLEAR_DEPTH,
      1.0f,
      0
      );
    }
  }


XAbstractShader *XD3DRenderer::getShader()
  {
  return 0;
  }

XAbstractGeometry *XD3DRenderer::getGeometry( XGeometry::BufferType )
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
