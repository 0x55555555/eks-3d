#ifndef XD3DRENDERER_H
#define XD3DRENDERER_H

#include "XMacroHelpers"
#include "X3DGlobal.h"

#if X_ENABLE_DX_RENDERER

struct IUnknown;
struct ID3D11Device1;
struct ID3D11DeviceContext1;

namespace Eks
{

class AllocatorBase;
class ScreenFrameBuffer;
class Renderer;

class EKS3D_EXPORT D3DRenderer
  {
public:
  static Renderer *createD3DRenderer(void *hwnd, ScreenFrameBuffer *buffer, Eks::AllocatorBase *);
  static Renderer *createD3DRenderer(IUnknown *window, ScreenFrameBuffer *buffer, Eks::AllocatorBase *);
  static void destroyD3DRenderer(Renderer *, ScreenFrameBuffer *buffer, Eks::AllocatorBase *);

  static void getInternals(Renderer *r, ID3D11Device1 **, ID3D11DeviceContext1 **ctx);

private:
  X_DISABLE_COPY(D3DRenderer);

  };

}

#endif


#endif // XD3DRENDERER_H
