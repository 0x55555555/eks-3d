#ifndef XD3DRENDERERIMPL_H
#define XD3DRENDERERIMPL_H

#include "XRenderer.h"

#if X_ENABLE_DX_RENDERER

#include "XMathMatrix"
#include "XColour"
#include <d3d11_1.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>
#include "wrl/client.h"

#include "XTexture.h"
#include "XRasteriserState.h"

namespace Eks
{

using Microsoft::WRL::ComPtr;

class XD3DShaderInputLayout
  {
public:
  ComPtr<ID3D11InputLayout> _inputLayout;
  };

class XD3DVertexShaderImpl
  {
public:
  bool create(ID3D11Device1 *device, const char *source, xsize length);

  ComPtr<ID3D11VertexShader> _vertexShader;
  };

class XD3DFragmentShaderImpl
  {
public:
  bool create(ID3D11Device1 *device, const char *source, xsize length);

  ComPtr<ID3D11PixelShader> _pixelShader;
  };

class XD3DSurfaceShaderImpl
  {
public:
  void bind(ID3D11DeviceContext1 *context) const;

  ComPtr<ID3D11VertexShader> _vertexShader;
  ComPtr<ID3D11PixelShader> _pixelShader;
  };

class XD3DRasteriserStateImpl
  {
public:
  bool create(ID3D11Device1 *context, const D3D11_RASTERIZER_DESC1& m);

  ComPtr<ID3D11RasterizerState1> _state;
  };

class XD3DDepthStencilStateImpl
  {
public:
  };

class XD3DBlendStateImpl
  {
public:
  };

class XD3DRenderTargetImpl
  {
public:
  bool create(ID3D11Device1 *context, Texture2D *col, Texture2D *depSte, DXGI_FORMAT depthRenderFormat);
  void clear(
      ID3D11DeviceContext1 *context,
      bool clearColour, bool clearDepth,
      const float *col,
      float depthVal,
      xuint8 stencilVal);
  void discard();

  ComPtr<ID3D11RenderTargetView> renderTargetView;
  ComPtr<ID3D11DepthStencilView> depthStencilView;
  };

class XD3DFrameBufferImpl : public XD3DRenderTargetImpl
  {
public:
  bool create(Renderer *r, ID3D11Device1 *context, IDXGISwapChain1 *swapChain);
  bool create(Renderer *r,
      ID3D11Device1 *context,
      xuint32 width,
      xuint32 height,
      DXGI_FORMAT colourFormat,
      xuint8 colourBpp,
      DXGI_FORMAT depthFormat,
      DXGI_FORMAT depthRenderFormat,
      DXGI_FORMAT depthShaderFormat,
      xuint8 depthBpp);

  Texture2D colour;
  Texture2D depthStencil;
  };

class XD3DBufferImpl
  {
public:
  bool create(ID3D11Device1 *dev, const void *data, xsize size, D3D11_BIND_FLAG type);
  void update(ID3D11DeviceContext1 *context, const void *data);
  Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
  };

class XD3DIndexBufferImpl : public XD3DBufferImpl
  {
public:
  DXGI_FORMAT format;
  xsize count;
  };

class XD3DShaderResourceImpl
  {
public:
  bool create(ID3D11Device1 *dev, const D3D11_SHADER_RESOURCE_VIEW_DESC *desc);
  Microsoft::WRL::ComPtr<ID3D11Resource> resource;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
  };

class XD3DTexture2DImpl : public XD3DShaderResourceImpl
  {
public:
  bool create(ID3D11Device1 *dev,
    xsize width,
    xsize height,
    DXGI_FORMAT format,
    DXGI_FORMAT shader,
    const void *data,
    xuint8 bpp,
    UINT bindFlags = D3D11_BIND_SHADER_RESOURCE,
    D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE);

  bool create(ID3D11Device1 *dev, IDXGISwapChain1 *swapChain);
  };

class XD3DVertexBufferImpl : public XD3DBufferImpl
  {
public:
  xsize elementSize;
  xsize elementCount;
  };

class XD3DSamplerImpl
  {
public:
  bool create(ID3D11Device *dev);
  ComPtr<ID3D11SamplerState> _sampler;
  };

template <typename T> class XD3DTypedBufferImpl : public XD3DBufferImpl
  {
public:
  void create(ID3D11Device1 *dev, D3D11_BIND_FLAG type)
    {
    XD3DBufferImpl::create(dev, 0, sizeof(T), type);
    }
  void update(ID3D11DeviceContext1 *context)
    {
    XD3DBufferImpl::update(context, &data);
    }

  T data;
  };

class XD3DSwapChainImpl : public XD3DFrameBufferImpl
  {
public:
  void present(ID3D11DeviceContext1 *context, bool *lostDevice);
  bool resize(
    Renderer *renderer,
    ID3D11Device1 *dev,
    IUnknown *window,
    HWND handle,
    xuint32 w,
    xuint32 h,
    int rotation);
  void discard();

  ComPtr<IDXGISwapChain1> swapChain;
  IUnknown *window;
  };

class D3DRendererImpl : public Renderer
  {
public:
  D3DRendererImpl(IUnknown *w, const detail::RendererFunctions &fn);
  D3DRendererImpl(HWND handle, const detail::RendererFunctions &fn);
  ~D3DRendererImpl();

  enum
    {
    UserVSContantBufferOffset = 2,
    UserPSContantBufferOffset = 0,
    UserVSResourceOffset = 0,
    UserPSResourceOffset = 0
    };

  // Direct3D Objects.
  D3D_FEATURE_LEVEL _featureLevel;
  ComPtr<ID3D11Device1> _d3dDevice;
  ComPtr<ID3D11DeviceContext1> _d3dContext;

  HWND _handle;
  IUnknown *_window;

  Eks::Colour _clearColour;

  struct WorldTransformData
    {
    Eks::Matrix4x4 view;
    Eks::Matrix4x4 projection;
    };
  bool _updateWorldTransformData;
  XD3DTypedBufferImpl<WorldTransformData> _worldTransformData;
  XD3DTypedBufferImpl<Eks::Matrix4x4> _modelTransformData;


  XD3DSamplerImpl _sampler;

  void setRenderTarget(XD3DRenderTargetImpl *target);
  void clearRenderTarget();
  bool createResources();
  bool resize(Renderer *renderer, XD3DSwapChainImpl *impl, xuint32 w, xuint32 h, xuint32 rotation);
  };

bool failedCheck(HRESULT res);

}

#endif

#endif // XD3DRENDERERIMPL_H
