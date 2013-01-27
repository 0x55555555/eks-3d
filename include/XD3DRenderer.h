#ifndef XD3DRENDERER_H
#define XD3DRENDERER_H

#include "X3DGlobal.h"

#if X_ENABLE_DX_RENDERER

struct IUnknown;

namespace Eks
{

class ScreenFrameBuffer;
class Renderer;

class EKS3D_EXPORT D3DRenderer
  {
public:
  static Renderer *createD3DRenderer(void *hwnd, ScreenFrameBuffer *buffer, Eks::AllocatorBase *);
  static Renderer *createD3DRenderer(IUnknown *window, ScreenFrameBuffer *buffer, Eks::AllocatorBase *);
  static void destroyD3DRenderer(Renderer *, ScreenFrameBuffer *buffer, Eks::AllocatorBase *);

private:
  X_DISABLE_COPY(D3DRenderer);

  };

}

#endif


#endif // XD3DRENDERER_H
