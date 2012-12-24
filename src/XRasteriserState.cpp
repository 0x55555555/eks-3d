#include "XRasteriserState.h"
#include "XRenderer.h"

namespace Eks
{

RasteriserState::RasteriserState(Renderer *r, CullMode cull)
  {
  if(r)
    {
    delayedCreate(*this, r, cull);
    }
  }

RasteriserState::~RasteriserState()
  {
  if(_renderer)
    {
    _renderer->destroyRasteriserState(this);
    }
  }

bool RasteriserState::delayedCreate(RasteriserState &ths, Renderer *r, CullMode cull)
  {
  ths._renderer = r;
  return r->createRasteriserState(&ths, cull);
  }

}
