#include "X3DCanvas.h"
#include "XD3DRenderer.h"
#include "XFramebuffer.h"

namespace Eks
{

#if X_ENABLE_GL_RENDERER

X3DCanvas::X3DCanvas(QWidget *parent) : QGLWidget(parent)
  {
  }

void X3DCanvas::paintGL()
  {
  paint();
  }

void X3DCanvas::update(XAbstractRenderModel::UpdateMode c)
  {
  X3DDataModelFunction
  XAbstractCanvas::update(c);
  QGLWidget::updateGL();
  }

bool X3DCanvas::isShown()
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
  _renderer = Eks::D3DRenderer::createD3DRenderer((void*)handle, _buffer);
  }

D3D3DCanvas::~D3D3DCanvas()
  {
  Eks::D3DRenderer::destroyD3DRenderer(_renderer, _buffer);
  delete _buffer;
  _buffer = 0;
  }

void D3D3DCanvas::resizeEvent(QResizeEvent* evt)
  {
  _buffer->resize(evt->size().width(), evt->size().height(), ScreenFrameBuffer::RotateNone);
  }

}

#endif
