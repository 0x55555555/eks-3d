#ifndef XD3DRENDERERIMPL_H
#define XD3DRENDERERIMPL_H

#include "XMathMatrix"
#include "XColour"
#include <d3d11_1.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>
#include "wrl/client.h"

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

class XD3DFrameBufferImpl
  {
public:
  void discard();
  bool create(ID3D11Device1 *context, IDXGISwapChain1 *swapChain);

  ComPtr<ID3D11Texture2D> colour;
  ComPtr<ID3D11Texture2D> depthStencil;
  };

class XD3DRenderTargetImpl
  {
public:
  bool create(ID3D11Device1 *context, XD3DFrameBufferImpl *fb);
  void clear(ID3D11DeviceContext1 *context,
             bool clearColour, bool clearDepth,
             const float *col,
             float depthVal,
             xuint8 stencilVal);
  void discard();

  ComPtr<ID3D11RenderTargetView> renderTargetView;
  ComPtr<ID3D11DepthStencilView> depthStencilView;
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

class XD3DVertexBufferImpl : public XD3DBufferImpl
  {
public:
  xsize elementSize;
  xsize elementCount;
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

class XD3DSwapChainImpl : public XD3DRenderTargetImpl
  {
public:
  void present(ID3D11DeviceContext1 *context, bool *lostDevice);
  bool resize(ID3D11Device1 *dev, IUnknown *window, xuint32 w, xuint32 h, int rotation);
  void discard();

  ComPtr<IDXGISwapChain1> swapChain;
  XD3DFrameBufferImpl framebuffer;
  IUnknown *window;
  };

class D3DRendererImpl
  {
public:
  D3DRendererImpl();
  ~D3DRendererImpl();

  // Direct3D Objects.
  D3D_FEATURE_LEVEL _featureLevel;
  ComPtr<ID3D11Device1> _d3dDevice;
  ComPtr<ID3D11DeviceContext1> _d3dContext;
  IUnknown *_window;

  XD3DSwapChainImpl _renderTarget;

  Eks::Colour _clearColour;

  struct WorldTransformData
    {
    Eks::Matrix4x4 view;
    Eks::Matrix4x4 projection;
    };
  bool _updateWorldTransformData;
  XD3DTypedBufferImpl<WorldTransformData> _worldTransformData;
  XD3DBufferImpl _modelTransformData;

  enum
    {
    TransformStackSize = 16,
    UserConstantBufferStartOffset = 2
    };

  Eks::Matrix4x4 _transformStack[TransformStackSize];
  Eks::Matrix4x4 *_currentTransform;

  void setRenderTarget(XD3DRenderTargetImpl *target);
  void clearRenderTarget();
  bool createResources();
  bool resize(xuint32 w, xuint32 h, int rotation);
  };

bool failedCheck(HRESULT res);

}

#endif // XD3DRENDERERIMPL_H
