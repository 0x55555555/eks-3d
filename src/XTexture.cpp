#include "XTexture.h"
#include "XRenderer.h"

namespace Eks
{

Texture2D::Texture2D(
    Renderer *r,
    xsize width,
    xsize height,
    Format fmt,
    void *data)
  {
  if(r)
    {
    delayedCreate(*this, r, width, height, fmt, data);
    }
  }

Texture2D::~Texture2D()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.texture2D(_renderer, this);
    }
  }

bool Texture2D::delayedCreate(
    Texture2D &ths,
    Renderer *r,
    xsize width,
    xsize height,
    Format fmt,
    void *data)
  {
  ths._renderer = r;
  return r->functions().create.texture2D(r, &ths, width, height, fmt, data);
  }

}
