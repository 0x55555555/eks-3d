#include "XD3DRendererImpl.h"

#ifdef X_ENABLE_DX_RENDERER

#include <comdef.h>
#include "Utilities/XAssert.h"
#include "Memory/XResourceDescription.h"
#include "Utilities/XOptional.h"

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

D3DRendererImpl::D3DRendererImpl(IUnknown *w, const detail::RendererFunctions& fns)
  {
  setFunctions(fns);
  _featureLevel = D3D_FEATURE_LEVEL_9_1;

  xAssertIsAligned(&_clearColour);
  _clearColour = Colour::Zero();
  _window = w;
  _handle = 0;
  createResources();
  }

D3DRendererImpl::D3DRendererImpl(HWND hwnd, const detail::RendererFunctions& fns)
  {
  setFunctions(fns);
  _featureLevel = D3D_FEATURE_LEVEL_9_1;

  xAssertIsAligned(&_clearColour);
  _clearColour = Colour::Zero();

  _window = 0;
  _handle = hwnd;
  createResources();
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
  _modelTransformData.create(_d3dDevice.Get(), D3D11_BIND_CONSTANT_BUFFER);

  _modelTransformData.data = Matrix4x4::Identity();

  _sampler.create(device.Get());

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

bool XD3DFrameBufferImpl::create(Renderer *r, ID3D11Device1 *dev, IDXGISwapChain1 *swapChain)
  {
  xAssert(swapChain);

  colour.setRenderer(r);
  depthStencil.setRenderer(r);

  XD3DTexture2DImpl *colImpl = colour.create<XD3DTexture2DImpl>();
  XD3DTexture2DImpl *depthImpl = depthStencil.create<XD3DTexture2DImpl>();

  colImpl->create(dev, swapChain);

  DXGI_SWAP_CHAIN_DESC1 swapDesc;
  swapChain->GetDesc1(&swapDesc);

  depthImpl->create(
        dev,
        swapDesc.Width,
        swapDesc.Height,
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        DXGI_FORMAT_UNKNOWN,
        0,
        32,
        D3D11_BIND_DEPTH_STENCIL,
        D3D11_USAGE_DEFAULT);


  return XD3DRenderTargetImpl::create(dev, &colour, &depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
  }

bool XD3DFrameBufferImpl::create(
  Renderer *r,
    ID3D11Device1 *dev,
    xuint32 width,
    xuint32 height,
    DXGI_FORMAT colourFormat,
    xuint8 colourBpp,
    DXGI_FORMAT depthFormat,
    DXGI_FORMAT depthRenderFormat,
    DXGI_FORMAT depthShaderFormat,
    xuint8 depthBpp)
  {

  colour.setRenderer(r);
  depthStencil.setRenderer(r);

  XD3DTexture2DImpl *colImpl = colour.create<XD3DTexture2DImpl>();
  XD3DTexture2DImpl *depthImpl = depthStencil.create<XD3DTexture2DImpl>();

  colImpl->create(
        dev,
        width,
        height,
        colourFormat,
        colourFormat,
        0,
        colourBpp,
        D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE,
        D3D11_USAGE_DEFAULT);

  depthImpl->create(
        dev,
        width,
        height,
        depthFormat,
        depthShaderFormat,
        0,
        depthBpp,
        D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE,
        D3D11_USAGE_DEFAULT);

  return XD3DRenderTargetImpl::create(dev, &colour, &depthStencil, depthRenderFormat);
  }

bool XD3DRenderTargetImpl::create(
    ID3D11Device1 *dev,
    Texture2D *col,
    Texture2D *depSte,
    DXGI_FORMAT depthRenderFormat)
  {
  XD3DTexture2DImpl *colT = col->data<XD3DTexture2DImpl>();
  XD3DTexture2DImpl *depSteT = depSte->data<XD3DTexture2DImpl>();
  if(failedCheck(dev->CreateRenderTargetView(
          colT->resource.Get(),
          nullptr,
          &renderTargetView
          )))
    {
    return false;
    }
  xAssert(renderTargetView);


  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc =
  {
    depthRenderFormat,
    D3D11_DSV_DIMENSION_TEXTURE2D,
    0
  };
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  if(failedCheck(dev->CreateDepthStencilView(
          depSteT->resource.Get(),
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

  CD3D11_BUFFER_DESC constantBufferDesc((UINT)size, type);
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
  Optional<bool> deviceLost(deviceListOptional);
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
    Renderer *renderer,
    ID3D11Device1 *dev,
    IUnknown *window,
    HWND handle,
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

    if(window)
      {
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
      }
    else if(handle)
      {
      if(failedCheck(dxgiFactory->CreateSwapChainForHwnd(
                     dev,
                     handle,
                     &swapChainDesc,
                     0,
                     0,
                     &swapChain
                     )))
        {
        return false;
        }
      }
    else
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

  if(!XD3DFrameBufferImpl::create(renderer, dev, swapChain.Get()))
    {
    return false;
    }

  return true;
  }

void XD3DSwapChainImpl::discard()
  {
  XD3DFrameBufferImpl::discard();

  if(colour.isValid())
    {
    colour.destroy<XD3DTexture2DImpl>();
    }

  if(depthStencil.isValid())
    {
    depthStencil.destroy<XD3DTexture2DImpl>();
    }
  }

bool D3DRendererImpl::resize(
    Renderer *renderer,
    XD3DSwapChainImpl *impl,
    xuint32 w,
    xuint32 h,
    xuint32 rotation)
  {
  // clear the state.
  clearRenderTarget();
  _d3dContext->ClearState();
  _d3dContext->Flush();

  impl->resize(renderer, _d3dDevice.Get(), _window, _handle, w, h, rotation);

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

bool XD3DShaderResourceImpl::create(ID3D11Device1 *dev, const D3D11_SHADER_RESOURCE_VIEW_DESC *desc)
  {
  if(failedCheck(dev->CreateShaderResourceView(resource.Get(), desc, &view)))
    {
    return false;
    }

  return true;
  }

bool XD3DTexture2DImpl::create(
    ID3D11Device1 *,
    IDXGISwapChain1 *swapChain)
  {
  // Create a render target view of the swap chain back buffer.
  ComPtr<ID3D11Texture2D> col;
  if(failedCheck(swapChain->GetBuffer(
          0,
          __uuidof(ID3D11Texture2D),
          (void **)&col
          )))
    {
    return false;
    }

  resource = col;
  return true;
  }

bool XD3DTexture2DImpl::create(
    ID3D11Device1 *dev,
    xsize width,
    xsize height,
    DXGI_FORMAT format,
    DXGI_FORMAT readFormat,
    const void *inp,
    xuint8 bpp,
    UINT bindFlags,
    D3D11_USAGE usage)
  {
  D3D11_TEXTURE2D_DESC desc;
  desc.Width = (UINT)width;
  desc.Height = (UINT)height;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = format;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = usage;
  desc.BindFlags = bindFlags;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA data;
  data.pSysMem = inp;
  data.SysMemPitch = (UINT)(width * bpp);
  data.SysMemSlicePitch = 0;

#ifdef X_DEBUG
  const xsize dataSize = width * height * bpp;
  const void *dataEnd = Eks::offsetBy(inp, dataSize);
  (void)dataEnd;
#endif

  ID3D11Texture2D *tex = 0;
  if(failedCheck(dev->CreateTexture2D(&desc, inp ? &data : 0, &tex)))
    {
    return false;
    }
  resource = tex;

  bool result = true;
  if((bindFlags&D3D11_BIND_SHADER_RESOURCE) != 0)
    {
    D3D11_SHADER_RESOURCE_VIEW_DESC rscDesc;
    rscDesc.Format = readFormat;
    rscDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    rscDesc.Texture2D.MipLevels = -1;
    rscDesc.Texture2D.MostDetailedMip = 0;

    result = XD3DShaderResourceImpl::create(dev, &rscDesc);
    }

  return result;
  }

bool XD3DSamplerImpl::create(ID3D11Device *dev)
  {
  D3D11_SAMPLER_DESC desc;

  desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.MipLODBias = 0;
  desc.MaxAnisotropy = 16;
  desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  desc.BorderColor[0] = 0.0f;
  desc.BorderColor[1] = 0.0f;
  desc.BorderColor[2] = 0.0f;
  desc.BorderColor[3] = 0.0f;
  desc.MinLOD = 0.0f;
  desc.MaxLOD = D3D11_FLOAT32_MAX;

  if(failedCheck(dev->CreateSamplerState(&desc, &_sampler)))
    {
    return false;
    }

  return true;
  }

}

#endif
