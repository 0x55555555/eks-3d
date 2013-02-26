#ifndef XBLENDSTATE_H
#define XBLENDSTATE_H

#include "X3DGlobal.h"
#include "XPrivateImpl"

namespace Eks
{

class Renderer;

class EKS3D_EXPORT BlendState : public PrivateImpl<sizeof(void *)>
  {
public:
  BlendState(Renderer *r=0);
  ~BlendState();

  static bool delayedCreate(
    BlendState &ths,
    Renderer *r);

private:
  Renderer *_renderer;
  };

#endif // XBLENDSTATE_H
