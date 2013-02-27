#include "XBlendState.h"
#include "XRenderer.h"

namespace Eks
{

BlendState::BlendState(Renderer *r)
  {
  if(r)
    {
    delayedCreate(*this, r);
    }
  }

BlendState::~BlendState()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.blendState(_renderer, this);
    }
  }

bool BlendState::delayedCreate(BlendState &ths, Renderer *r)
  {
  ths._renderer = r;
  return r->functions().create.blendState(r, &ths);
  }

}
