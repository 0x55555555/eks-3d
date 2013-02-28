#ifndef XFRAMEBUFFER_H
#define XFRAMEBUFFER_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XPrivateImpl"
#include "XTexture.h"

namespace Eks
{

class Renderer;
class FrameBufferRenderFrame;

class EKS3D_EXPORT FrameBuffer : public PrivateImpl<sizeof(void *) * 12>
  {
public:
  typedef FrameBufferRenderFrame RenderFrame;

  enum TextureId
    {
    TextureColour,
    TextureDepthStencil,

    TextureIdCount
    };

  FrameBuffer(
    Renderer *r = 0,
    xuint32 width = 0,
    xuint32 height = 0,
    Texture2D::Format colour = Texture2D::Rgba8,
    Texture2D::Format dsF = Texture2D::Depth24Stencil8);
  ~FrameBuffer();

  static bool delayedCreate(
    FrameBuffer &ths,
    Renderer *r,
    xuint32 width,
    xuint32 height,
    Texture2D::Format colour = Texture2D::Rgba8,
    Texture2D::Format dsF = Texture2D::Depth24Stencil8);

  enum ClearMode
    {
    ClearColour = 1,
    ClearDepth = 2
    };

  void clear(xuint32 mode);

  Texture2D *getTexture(TextureId id);

protected:
  Renderer *_renderer;
  };

class EKS3D_EXPORT ScreenFrameBuffer : public FrameBuffer
  {
public:
  ScreenFrameBuffer();
  ~ScreenFrameBuffer();

  enum Rotation
    {
    RotateNone,
    Rotate90,
    Rotate180,
    Rotate270
    };

  void setRenderer(Renderer *r);

  void present(bool *deviceLost);
  bool resize(xuint32 w, xuint32 h, Rotation rotation);

private:
  // trying to hide the parent method
  static void delayedCreate() { }

  X_DISABLE_COPY(ScreenFrameBuffer)
  };

class EKS3D_EXPORT FrameBufferRenderFrame
  {
public:
  FrameBufferRenderFrame(Renderer *r, FrameBuffer *buffer);
  ~FrameBufferRenderFrame();

private:
  X_DISABLE_COPY(FrameBufferRenderFrame)

  FrameBuffer *_framebuffer;
  Renderer *_renderer;
  };
}

#include "XRenderer.h"

namespace Eks
{

inline void FrameBuffer::clear(xuint32 mode)
  {
  xAssert(_renderer);
  _renderer->functions().frame.clear(_renderer, this, mode);
  }

inline Texture2D *FrameBuffer::getTexture(TextureId tex)
  {
  return _renderer->functions().frame.getTexture(_renderer, this, tex);
  }

}

#endif // XFRAMEBUFFER_H
