#include "XBlendState.h"
#include "XRenderer.h"

namespace Eks
{

BlendState::BlendState(
    Renderer *r,
    bool enable,
    BlendMode modeRGB,
    BlendParameter srcRGB,
    BlendParameter dstRGB,
    BlendMode modeAlpha,
    BlendParameter srcAlpha,
    BlendParameter dstAlpha,
    const Eks::Colour &col)
  {
  if(r)
    {
    delayedCreate(*this, r, enable, modeRGB, srcRGB, dstRGB, modeAlpha, srcAlpha, dstAlpha, col);
    }
  }

BlendState::~BlendState()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.blendState(_renderer, this);
    }
  }

bool BlendState::delayedCreate(
    BlendState &ths,
    Renderer *r,
    bool enable,
    BlendMode modeRGB,
    BlendParameter srcRGB,
    BlendParameter dstRGB,
    BlendMode modeAlpha,
    BlendParameter srcAlpha,
    BlendParameter dstAlpha,
    const Colour &col)
  {
  ths._renderer = r;
  return r->functions().create.blendState(
    r,
    &ths,
    enable,
    modeRGB,
    srcRGB,
    dstRGB,
    modeAlpha,
    srcAlpha,
    dstAlpha,
    col);
  }

}
