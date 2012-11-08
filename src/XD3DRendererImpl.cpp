#include "XD3DRendererImpl.h"
#include "XAssert"

bool XD3DRendererImpl::createResources()
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

  ID3D11DeviceContext *context = 0;

  // Create the Direct3D 11 API device object and a corresponding context.
  if(FAILED(
       D3D11CreateDevice(
         nullptr, // Specify nullptr to use the default adapter.
         D3D_DRIVER_TYPE_HARDWARE,
         nullptr,
         creationFlags, // Set set debug and Direct2D compatibility flags.
         featureLevels, // List of feature levels this app can support.
         X_ARRAY_COUNT(featureLevels),
         D3D11_SDK_VERSION, // Always set this to D3D11_SDK_VERSION for Windows Store apps.
         &_d3dDevice, // Returns the Direct3D device created.
         &_featureLevel, // Returns feature level of device created.
         &context // Returns the device immediate context.
         )
       ))
    {
    xAssertFail();
    return false;
    }

  if(FAILED(context->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&_d3dContext)))
    {
    xAssertFail();
    return false;
    }

  return true;
  }

bool XD3DRendererImpl::resize(xuint32 w, xuint32 h, int rotation)
  {
  if(_swapChain != nullptr)
    {
    // If the swap chain already exists, resize it.
    if(FAILED(_swapChain->ResizeBuffers(
           2, // Double-buffered swap chain.
           w,
           h,
           DXGI_FORMAT_B8G8R8A8_UNORM,
           0
           )))
      {
      xAssertFail();
      return false;
      }
    }
  else
    {
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    _d3dContext->OMSetRenderTargets(X_ARRAY_COUNT(nullViews), nullViews, nullptr);
    _renderTargetView = nullptr;
    _depthStencilView = nullptr;
    _d3dContext->Flush();

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

    IDXGIDevice1 *dxgiDevice = 0;
    if(FAILED(_d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice)))
      {
      xAssertFail();
      return false;
      }

    IDXGIAdapter *dxgiAdapter = 0;
    if(FAILED(dxgiDevice->GetAdapter(&dxgiAdapter)))
      {
      xAssertFail();
      return false;
      }

    IDXGIFactory2 *dxgiFactory = 0;
    if(FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory)))
      {
      xAssertFail();
      return false;
      }

    if(FAILED(dxgiFactory->CreateSwapChainForCoreWindow(
                _d3dDevice,
                _window,
                &swapChainDesc,
                nullptr, // Allow on all displays.
                &_swapChain
                )))
      {
      xAssertFail();
      return false;
      }

    // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
    // ensures that the application will only render after each VSync, minimizing power consumption.
    if(FAILED(dxgiDevice->SetMaximumFrameLatency(1)))
      {
      xAssertFail();
      return false;
      }
    }

  DXGI_MODE_ROTATION rotationConv = (DXGI_MODE_ROTATION)(rotation + 1);

  IDXGISwapChain1 *swapChain = static_cast<IDXGISwapChain1 *>(_swapChain);
  if(FAILED(swapChain->SetRotation(rotationConv)))
    {
    xAssertFail();
    return false;
    }

  // Create a render target view of the swap chain back buffer.
  ID3D11Texture2D *backBuffer = 0;
  if(FAILED(_swapChain->GetBuffer(
          0,
          __uuidof(ID3D11Texture2D),
          (void**)&backBuffer
          )))
    {
    xAssertFail();
    return false;
    }

  if(FAILED(_d3dDevice->CreateRenderTargetView(
          backBuffer,
          nullptr,
          &_renderTargetView
          )))
  {
  xAssertFail();
  return false;
  }

  // Create a depth stencil view.
  CD3D11_TEXTURE2D_DESC depthStencilDesc(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        static_cast<UINT>(w),
        static_cast<UINT>(h),
        1,
        1,
        D3D11_BIND_DEPTH_STENCIL
        );

  ID3D11Texture2D *depthStencil = 0;
  if(FAILED(_d3dDevice->CreateTexture2D(
          &depthStencilDesc,
          nullptr,
          &depthStencil
          )))
    {
    xAssertFail();
    return false;
    }

  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
  if(FAILED(_d3dDevice->CreateDepthStencilView(
          depthStencil,
          &depthStencilViewDesc,
          &_depthStencilView
          )))
    {
    xAssertFail();
    return false;
    }

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
