#ifndef XDEPTHSTENCILSTATE_H
#define XDEPTHSTENCILSTATE_H

#include "X3DGlobal.h"
#include "XPrivateImpl"

namespace Eks
{

class Renderer;

class EKS3D_EXPORT DepthStencilState : public PrivateImpl<sizeof(void *)>
  {
public:
  DepthStencilState(Renderer *r=0);
  ~DepthStencilState();

  static bool delayedCreate(
    DepthStencilState &ths,
    Renderer *r);

private:
  Renderer *_renderer;
  };

#endif // XDEPTHSTENCILSTATE_H
