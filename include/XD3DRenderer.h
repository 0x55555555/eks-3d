#ifndef XD3DRENDERER_H
#define XD3DRENDERER_H

#include "X3DGlobal.h"

struct IUnknown;

namespace Eks
{

class Renderer;

class EKS3D_EXPORT D3DRenderer
  {
public:
  static Renderer *createD3DRenderer(IUnknown *window);
  static void destroyD3DRenderer(Renderer *);

private:
  X_DISABLE_COPY(D3DRenderer);

  };

}

#endif // XD3DRENDERER_H
