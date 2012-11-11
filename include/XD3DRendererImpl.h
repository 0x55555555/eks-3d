#ifndef XD3DRENDERERIMPL_H
#define XD3DRENDERERIMPL_H

#include "XColour"
#include <d3d11_1.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>

class XD3DRendererImpl
  {
public:
  XD3DRendererImpl();

  // Direct3D Objects.
  D3D_FEATURE_LEVEL _featureLevel;
  ID3D11Device1 *_d3dDevice;
  ID3D11DeviceContext1 *_d3dContext;
  IDXGISwapChain1 *_swapChain;
  ID3D11RenderTargetView *_renderTargetView;
  ID3D11DepthStencilView *_depthStencilView;
  IUnknown *_window;

  XColour _clearColour;

  bool createResources();
  bool resize(xuint32 w, xuint32 h, int rotation);
  };

bool failedCheck(HRESULT res);

#endif // XD3DRENDERERIMPL_H
