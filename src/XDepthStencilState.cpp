#include "XDepthStencilState.h"
#include "XRenderer.h"

namespace Eks
{

DepthStencilState::DepthStencilState(Renderer *r,
     xuint32 writeMask,
     xuint32 tests,
     Function depthTest,
     Function stencilTest,
     xint32 stencilRef,
     xuint32 stencilMask,
     float depthNear,
     float depthFar)
  {
  if(r)
    {
    delayedCreate(
      *this,
      r,
      writeMask,
      tests,
      depthTest,
      stencilTest,
      stencilRef,
      stencilMask,
      depthNear,
      depthFar);
    }
  }

DepthStencilState::~DepthStencilState()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.depthStencilState(_renderer, this);
    }
  }

bool DepthStencilState::delayedCreate(
    DepthStencilState &ths,
    Renderer *r,
    xuint32 writeMask,
    xuint32 tests,
    Function depthTest,
    Function stencilTest,
    xint32 stencilRef,
    xuint32 stencilMask,
    float depthNear,
    float depthFar)
  {
  ths._renderer = r;
  return r->functions().create.depthStencilState(
    r,
    &ths,
    writeMask,
    tests,
    depthTest,
    stencilTest,
    stencilRef,
    stencilMask,
    depthNear,
    depthFar);
  }

}
