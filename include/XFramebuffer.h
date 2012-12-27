#ifndef XFRAMEBUFFER_H
#define XFRAMEBUFFER_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XPrivateImpl"

namespace Eks
{

class Renderer;
class FrameBufferRenderFrame;

class FrameBuffer : public PrivateImpl<sizeof(void *) * 6>
  {
public:
  typedef FrameBufferRenderFrame RenderFrame;

  enum ColourFormat
    {
    ColourRgb8,

    ColourFormatCount };

  enum DepthStencilFormat
    {
    DepthNoneStencilNone,
    Depth24StencilNone,
    Depth24Stencil8,

    DepthStencilFormatCount
    };

  FrameBuffer(
    Renderer *r = 0,
    xuint32 width = 0,
    xuint32 height = 0,
    ColourFormat colour = ColourRgb8,
    DepthStencilFormat dsF = Depth24Stencil8);
  ~FrameBuffer();

  static bool delayedCreate(
    FrameBuffer &ths,
    Renderer *r,
    xuint32 width,
    xuint32 height,
    ColourFormat colour = ColourRgb8,
    DepthStencilFormat ds = Depth24Stencil8);

  enum ClearMode
    {
    ClearColour = 1,
    ClearDepth = 2
    };

  void clear(xuint32 mode);

protected:
  Renderer *_renderer;
  };

class ScreenFrameBuffer : public FrameBuffer
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
  static bool delayedCreate() { }

  X_DISABLE_COPY(ScreenFrameBuffer)
  };

class FrameBufferRenderFrame
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

#endif // XFRAMEBUFFER_H
