#ifndef XABSTRACTTEXTURE_H
#define XABSTRACTTEXTURE_H

#include "X3DGlobal.h"
#include "XPrivateImpl"
#include "XProperty"
#include "XRenderer.h"

namespace Eks
{

class Renderer;

class EKS3D_EXPORT Resource : public PrivateImpl<sizeof(void *) * 4>
  {
protected:
  Resource();

private:
  X_DISABLE_COPY(Resource)
  };

class EKS3D_EXPORT Texture2D : public Resource
  {
public:
  Texture2D(Renderer *r=0,
        xsize width=0,
        xsize height=0,
        TextureFormat fmt=Eks::TextureFormatCount,
        const void *data=0);
  ~Texture2D();

  static bool delayedCreate(
        Texture2D &ths,
        Renderer *r,
        xsize width,
        xsize height,
        TextureFormat fmt,
        const void *data);

  Eks::VectorUI2D size() const;

  void setRenderer(Renderer *r);

private:
  X_DISABLE_COPY(Texture2D)

  Renderer *_renderer;
  };


inline Eks::VectorUI2D Texture2D::size() const
  {
  xAssert(isValid());
  Eks::VectorUI2D ret;
  _renderer->functions().get.texture2DInfo(_renderer, this, ret);

  return ret;
  }
}

#endif // XABSTRACTTEXTURE_H
