#ifndef XRASTERISERSTATE_H
#define XRASTERISERSTATE_H

#include "XPrivateImpl"

namespace Eks
{

class Renderer;

class RasteriserState : public PrivateImpl<sizeof(void *)>
  {
public:
  enum CullMode
    {
    CullNone,
    CullBack,
    CullFront,

    CullModeCount
    };

  RasteriserState(Renderer *r=0, CullMode cull=CullBack);
  ~RasteriserState();


  static bool delayedCreate(
    RasteriserState &ths,
    Renderer *r,
    CullMode cull);

private:
  Renderer *_renderer;
  };

}

#endif // XRASTERISERSTATE_H
