#include "XDepthStencilState.h"
#include "XRenderer.h"

namespace Eks
{

DepthStencilState::DepthStencilState(Renderer *r)
  {
  if(r)
    {
    delayedCreate(*this, r);
    }
  }

DepthStencilState::~DepthStencilState()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.depthStencilState(_renderer, this);
    }
  }

bool DepthStencilState::delayedCreate(DepthStencilState &ths, Renderer *r)
  {
  ths._renderer = r;
  return r->functions().create.depthStencilState(r, &ths);
  }

}
