#include <d3d11.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>
#include "XD3DRenderer.h"
#include "XD3DRendererImpl.h"
#include "XColour"
#include "XOptional"
#include "XShader.h"

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
  if(_impl->_updateWorldTransformData)
    {
    _impl->_worldTransformData.update(_impl->_d3dContext.Get());
    }

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

void XD3DRenderer::pushTransform(const Transform &tr)
  {
  xAssert((_impl->_currentTransform - _impl->_transformStack) <
            XD3DRendererImpl::TransformStackSize);

  XMatrix4x4& oldTransform = *_impl->_currentTransform;
  XMatrix4x4& newTransform = *(++_impl->_currentTransform);

  newTransform = (oldTransform * tr).matrix();
  _impl->_modelTransformData.update(_impl->_d3dContext.Get(), newTransform.transpose().data());
  }

void XD3DRenderer::popTransform( )
  {
  _impl->_currentTransform--;
  xAssert(_impl->_currentTransform >= _impl->_transformStack);
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

bool XD3DRenderer::createShader(XShader *s, XShaderVertexComponent *v, XShaderFragmentComponent *f)
  {
  XD3DFragmentShaderImpl *frag = f->data<XD3DFragmentShaderImpl>();
  XD3DVertexShaderImpl *vert = v->data<XD3DVertexShaderImpl>();

  XD3DSurfaceShaderImpl* shd = s->data<XD3DSurfaceShaderImpl>();
  new(shd) XD3DSurfaceShaderImpl();

  shd->_pixelShader = frag->_pixelShader;
  shd->_vertexShader = vert->_vertexShader;

  return shd->_pixelShader && shd->_vertexShader;
  }

bool XD3DRenderer::createVertexShaderComponent(XShaderVertexComponent *v, const char *s, xsize l)
  {
  XD3DVertexShaderImpl *vert = v->data<XD3DVertexShaderImpl>();
  new(vert) XD3DVertexShaderImpl();
  return vert->create(_impl->_d3dDevice.Get(), s, l);
  }

bool XD3DRenderer::createFragmentShaderComponent(XShaderFragmentComponent *f, const char *s, xsize l)
  {
  XD3DFragmentShaderImpl *frag = f->data<XD3DFragmentShaderImpl>();
  new(frag) XD3DFragmentShaderImpl();
  return frag->create(_impl->_d3dDevice.Get(), s, l);
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

void XD3DRenderer::destroyShader(XShader* s)
  {
  XD3DSurfaceShaderImpl* shd = s->data<XD3DSurfaceShaderImpl>();
  shd->_pixelShader = nullptr;
  shd->_vertexShader = nullptr;
  }

void XD3DRenderer::destroyVertexShaderComponent(XShaderVertexComponent* s)
  {
  XD3DVertexShaderImpl *vert = s->data<XD3DVertexShaderImpl>();
  vert->_vertexShader = nullptr;
  }

void XD3DRenderer::destroyFragmentShaderComponent(XShaderFragmentComponent* s)
  {
  XD3DFragmentShaderImpl *vert = s->data<XD3DFragmentShaderImpl>();
  vert->_pixelShader = nullptr;
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

void XD3DRenderer::setViewTransform(const XTransform &v)
  {
  _impl->_worldTransformData.data.view = v.matrix().transpose();
  _impl->_updateWorldTransformData = true;
  }

void XD3DRenderer::setProjectionTransform(const XComplexTransform &p)
  {
  _impl->_worldTransformData.data.projection = p.matrix().transpose();
  _impl->_updateWorldTransformData = true;
  }

void XD3DRenderer::setShader( const XShader *s )
  {
  const XD3DSurfaceShaderImpl* shd = s->data<XD3DSurfaceShaderImpl>();
  shd->bind(_impl->_d3dContext.Get());

  ID3D11Buffer *buffers[] =
  {
    _impl->_worldTransformData.buffer.Get(),
    _impl->_modelTransformData.buffer.Get()
  };

  _impl->_d3dContext->VSSetConstantBuffers(
    0,
    X_ARRAY_COUNT(buffers),
    buffers
    );
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
