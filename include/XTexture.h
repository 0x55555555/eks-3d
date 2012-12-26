#ifndef XABSTRACTTEXTURE_H
#define XABSTRACTTEXTURE_H

#include "X3DGlobal.h"
#include "XPrivateImpl"
#include "XProperty"

namespace Eks
{

class Renderer;

class Resource : public PrivateImpl<sizeof(void *) * 2>
  {
  };

class Texture2D : public Resource
  {
public:
  enum Format
    {
    Rgba8,

    FormatCount
    };

  Texture2D(
        Renderer *r=0,
        xsize width=0,
        xsize height=0,
        Format fmt=FormatCount,
        void *data=0);
  ~Texture2D();

  static bool delayedCreate(
        Texture2D &ths,
        Renderer *r,
        xsize width,
        xsize height,
        Format fmt,
        void *data);

private:
  Renderer *_renderer;
  };

}

#endif // XABSTRACTTEXTURE_H
