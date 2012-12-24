#include "XD3DRendererImpl.h"
#include <comdef.h>
#include "XAssert"
#include "XOptional"

namespace Eks
{

bool failedCheck(HRESULT res)
  {
  if(SUCCEEDED(res))
    {
    return false;
    }

  _com_error err(res, 0);
  LPCTSTR errMsg = err.ErrorMessage();
  (void)errMsg;

  return true;
  }

D3DRendererImpl::D3DRendererImpl()
  {
  _featureLevel = D3D_FEATURE_LEVEL_9_1;
  _window = 0;

  _currentTransform = _transformStack;
  *_currentTransform = Eks::Matrix4x4::Identity();
  }

D3DRendererImpl::~D3DRendererImpl()
  {
  }

void D3DRendererImpl::setRenderTarget(XD3DRenderTargetImpl *target)
  {
  ID3D11RenderTargetView* views[] = { target->renderTargetView.Get() };
  _d3dContext->OMSetRenderTargets(1, views, target->depthStencilView.Get());
  }

void D3DRendererImpl::clearRenderTarget()
  {
  ID3D11RenderTargetView* views[] = { nullptr };
  _d3dContext->OMSetRenderTargets(X_ARRAY_COUNT(views), views, 0);
  }

bool D3DRendererImpl::createResources()
  {
  // This flag adds support for surfaces with a different color channel ordering
  // than the API default. It is required for compatibility with Direct2D.
  UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(X_DEBUG)
  // If the project is in a debug build, enable debugging via SDK Layers with this flag.
  creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // This array defines the set of DirectX hardware feature levels this app will support.
  // Note the ordering should be preserved.
  // Don't forget to declare your application's minimum required feature level in its
  // description.  All applications are assumed to support 9.1 unless otherwise stated.
  D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1
  };

  // Create the Direct3D 11 API device object and a corresponding context.
  ComPtr<ID3D11Device> device;
  ComPtr<ID3D11DeviceContext> context;
  if(failedCheck(
       D3D11CreateDevice(
         nullptr, // Specify nullptr to use the default adapter.
         D3D_DRIVER_TYPE_HARDWARE,
         nullptr,
         creationFlags, // Set set debug and Direct2D compatibility flags.
         featureLevels, // List of feature levels this app can support.
         X_ARRAY_COUNT(featureLevels),
         D3D11_SDK_VERSION, // Always set this to D3D11_SDK_VERSION for Windows Store apps.
         &device, // Returns the Direct3D device created.
         &_featureLevel, // Returns feature level of device created.
         &context // Returns the device immediate context.
         )
       ))
    {
    return false;
    }

  if(failedCheck(device.As(&_d3dDevice)))
    {
    return false;
    }

  if(failedCheck(context.As(&_d3dContext)))
    {
    return false;
    }

  _worldTransformData.create(_d3dDevice.Get(), D3D11_BIND_CONSTANT_BUFFER);
  _modelTransformData.create(_d3dDevice.Get(),
                             0,
                             sizeof(D3DRendererImpl::_transformStack[0]),
                             D3D11_BIND_CONSTANT_BUFFER);

  return true;
  }

bool XD3DVertexShaderImpl::create(ID3D11Device1 *device, const char *source, xsize length)
  {
  if(failedCheck(device->CreateVertexShader(
      source,
      length,
      nullptr,
      &_vertexShader
      )))
    {
    return false;
    }
  return true;
  }

bool XD3DFragmentShaderImpl::create(ID3D11Device1 *device, const char *source, xsize length)
  {
  if(failedCheck(device->CreatePixelShader(
      source,
      length,
      nullptr,
      &_pixelShader
      )))
    {
    return false;
    }
  return true;
  }

void XD3DSurfaceShaderImpl::bind(ID3D11DeviceContext1 *context) const
  {
  context->VSSetShader(
    _vertexShader.Get(),
    nullptr,
    0
    );

  context->PSSetShader(
    _pixelShader.Get(),
    nullptr,
    0
    );
  }

bool XD3DRasteriserStateImpl::create(ID3D11Device1 *device, const D3D11_RASTERIZER_DESC1 &m)
  {
  return device->CreateRasterizerState1( &m, &_state );
  }

void XD3DFrameBufferImpl::discard()
  {
  colour = nullptr;
  depthStencil = nullptr;
  }

bool XD3DFrameBufferImpl::create(ID3D11Device1 *dev, IDXGISwapChain1 *swapChain)
  {
  xAssert(swapChain);


  // Create a render target view of the swap chain back buffer.
  if(failedCheck(swapChain->GetBuffer(
          0,
          __uuidof(ID3D11Texture2D),
          &colour
          )))
    {
    return false;
    }

  DXGI_SWAP_CHAIN_DESC1 swapDesc;
  swapChain->GetDesc1(&swapDesc);

  // Create a depth stencil view.
  CD3D11_TEXTURE2D_DESC depthStencilDesc(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        swapDesc.Width,
        swapDesc.Height,
        1,
        1,
        D3D11_BIND_DEPTH_STENCIL
        );

  if(failedCheck(dev->CreateTexture2D(
          &depthStencilDesc,
          nullptr,
          &depthStencil
          )))
    {
    return false;
    }

  return true;
  }

bool XD3DRenderTargetImpl::create(ID3D11Device1 *dev, XD3DFrameBufferImpl *fb)
  {
  if(failedCheck(dev->CreateRenderTargetView(
          fb->colour.Get(),
          nullptr,
          &renderTargetView
          )))
    {
    return false;
    }
  xAssert(renderTargetView);


  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
  if(failedCheck(dev->CreateDepthStencilView(
          fb->depthStencil.Get(),
          &depthStencilViewDesc,
          &depthStencilView
          )))
    {
    return false;
    }
  xAssert(depthStencilView);

  return true;
  }

void XD3DRenderTargetImpl::discard()
  {
  renderTargetView = nullptr;
  depthStencilView = nullptr;
  }

void XD3DRenderTargetImpl::clear(
    ID3D11DeviceContext1 *context,
    bool clearColour,
    bool clearDepth,
    const float *col,
    float depthVal,
    xuint8 stencilVal)
  {
  xAssert(renderTargetView);
  xAssert(depthStencilView);

  if(clearColour)
    {
    context->ClearRenderTargetView(
      renderTargetView.Get(),
      col
      );
    }

  if(clearDepth)
    {
    context->ClearDepthStencilView(
      depthStencilView.Get(),
      D3D11_CLEAR_DEPTH,
      depthVal,
      stencilVal
      );
    }
  }

bool XD3DBufferImpl::create(ID3D11Device1 *dev, const void *data, xsize size, D3D11_BIND_FLAG type)
  {
  D3D11_SUBRESOURCE_DATA bufferData = {0};
  bufferData.pSysMem = data;
  bufferData.SysMemPitch = 0;
  bufferData.SysMemSlicePitch = 0;

  CD3D11_BUFFER_DESC constantBufferDesc(size, type);
  if(failedCheck(
    dev->CreateBuffer(
      &constantBufferDesc,
      data ? &bufferData : 0,
      &buffer
      )
    ))
    {
    return false;
    }
  return true;
  }

void XD3DBufferImpl::update(ID3D11DeviceContext1 *context, const void *data)
  {
  context->UpdateSubresource(
    buffer.Get(),
    0,
    NULL,
    data,
    0,
    0
    );
  }

void XD3DSwapChainImpl::present(ID3D11DeviceContext1 *context, bool *deviceListOptional)
  {
  XOptional<bool> deviceLost(deviceListOptional);
  deviceLost = false;

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
  HRESULT hr = swapChain->Present1(1, 0, &parameters);

  // Discard the contents of the render target.
  // This is a valid operation only when the existing contents will be entirely
  // overwritten. If dirty or scroll rects are used, this call should be removed.
  context->DiscardView(renderTargetView.Get());

  // Discard the contents of the depth stencil.
  context->DiscardView(depthStencilView.Get());

  // If the device was removed either by a disconnect or a driver upgrade, we
  // must recreate all device resources.
  if (hr == DXGI_ERROR_DEVICE_REMOVED)
    {
    deviceLost = true;
    }
  else
    {
    failedCheck(hr);
    }
  }

bool XD3DSwapChainImpl::resize(
    ID3D11Device1 *dev,
    IUnknown *window,
    xuint32 w,
    xuint32 h,
    int rotation)
  {
  discard();
  if(swapChain != nullptr)
    {
    // If the swap chain already exists, resize it.
    if(failedCheck(swapChain->ResizeBuffers(
           2, // Double-buffered swap chain.
           w,
           h,
           DXGI_FORMAT_B8G8R8A8_UNORM,
           0
           )))
      {
      return false;
      }
    }
  else
    {
    ComPtr<ID3D11Device1> device = dev;
    ComPtr<IDXGIDevice1> dxgiDevice;
    if(failedCheck(device.As(&dxgiDevice)))
      {
      return false;
      }


    ComPtr<IDXGIAdapter> dxgiAdapter;
    if(failedCheck(dxgiDevice->GetAdapter(&dxgiAdapter)))
      {
      return false;
      }

    ComPtr<IDXGIFactory2> dxgiFactory;
    if(failedCheck(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory)))
      {
      return false;
      }

    // Otherwise, create a new one using the same adapter as the existing Direct3D device.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
    swapChainDesc.Width = w; // Match the size of the window.
    swapChainDesc.Height = h;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
    swapChainDesc.Flags = 0;

    if(failedCheck(dxgiFactory->CreateSwapChainForCoreWindow(
                dev,
                window,
                &swapChainDesc,
                nullptr, // Allow on all displays.
                &swapChain
                )))
      {
      return false;
      }

    // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
    // ensures that the application will only render after each VSync, minimizing power consumption.
    if(failedCheck(dxgiDevice->SetMaximumFrameLatency(1)))
      {
      return false;
      }
    }

  DXGI_MODE_ROTATION rotationConv = (DXGI_MODE_ROTATION)(rotation + 1);
  if(failedCheck(swapChain->SetRotation(rotationConv)))
    {
    return false;
    }

  if(!framebuffer.create(dev, swapChain.Get()))
    {
    return false;
    }

  if(!create(dev, &framebuffer))
    {
    return false;
    }

  return true;
  }

void XD3DSwapChainImpl::discard()
  {
  XD3DRenderTargetImpl::discard();
  framebuffer.discard();
  }

bool D3DRendererImpl::resize(xuint32 w, xuint32 h, int rotation)
  {
  // clear the state.
  clearRenderTarget();
  _d3dContext->ClearState();
  _d3dContext->Flush();

  _renderTarget.resize(_d3dDevice.Get(), _window, w, h, rotation);

  // Set the rendering viewport to target the entire window.
  CD3D11_VIEWPORT viewport(
        0.0f,
        0.0f,
        w,
        h
        );

  _d3dContext->RSSetViewports(1, &viewport);
  return true;
  }

}
