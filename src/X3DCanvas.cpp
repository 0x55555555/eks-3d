#include "X3DCanvas.h"

#if X_QT_INTEROP

#include "XD3DRenderer.h"
#include "XFramebuffer.h"

namespace Eks
{

#if X_ENABLE_GL_RENDERER

GL3DCanvas::GL3DCanvas(QWidget *parent) : QGLWidget(parent)
  {
  }

bool GL3DCanvas::isShown()
  {
  return !isHidden() && hasFocus();
  }

#endif

#if X_ENABLE_DX_RENDERER

D3D3DCanvas::D3D3DCanvas(QWidget* parent)
    : QWidget(parent)
  {
  setAttribute(Qt::WA_PaintOnScreen, true);
  setAttribute(Qt::WA_NativeWindow, true);

  WId handle = winId();

  _buffer = new ScreenFrameBuffer();
  _renderer = Eks::D3DRenderer::createD3DRenderer((void*)handle, _buffer, Eks::GlobalAllocator::instance());
  }

D3D3DCanvas::~D3D3DCanvas()
  {
  Eks::D3DRenderer::destroyD3DRenderer(_renderer, _buffer, Eks::GlobalAllocator::instance());
  delete _buffer;
  _buffer = 0;
  }

void D3D3DCanvas::resizeEvent(QResizeEvent* evt)
  {
  _buffer->resize(evt->size().width(), evt->size().height(), ScreenFrameBuffer::RotateNone);
  }

void D3D3DCanvas::paintEvent(QPaintEvent *)
  {
  paint3D(_renderer, _buffer);
  }

#endif


Canvas3D::Canvas3D(QWidget *w) : Base(w)
  {
  }

bool Canvas3D::isShown()
  {
  return isVisible();
  }

void Canvas3D::update(AbstractRenderModel::UpdateMode)
  {
  }

}

#endif
