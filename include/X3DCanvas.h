#ifndef X3DCANVAS_H
#define X3DCANVAS_H

#include "XAbstractCanvas.h"
#include "XAbstractCanvasController.h"
#include "QMouseEvent"

#if X_ENABLE_GL_RENDERER

#include "QGLWidget"

class EKS3D_EXPORT X3DGLCanvas : public QGLWidget, public XAbstractCanvas
  {
public:
  X3DCanvas(QWidget *parent=0);

  virtual void paintGL() X_OVERRIDE
    {
    paint3D();
    }
  virtual void paint3D() = 0;

  virtual bool isShown();
  };

#endif

#if X_ENABLE_DX_RENDERER

#include "QtWidgets/QWidget"

class QPaintEngine;

namespace Eks
{

class ScreenFrameBuffer;
class Renderer;

class EKS3D_EXPORT D3D3DCanvas : public QWidget
  {
public:

  D3D3DCanvas(QWidget* parent = 0);
  ~D3D3DCanvas();

  QPaintEngine* paintEngine() const X_OVERRIDE
    {
    return 0;
    }

protected:
  void resizeEvent(QResizeEvent* evt) X_OVERRIDE;
  void paintEvent(QPaintEvent*) X_OVERRIDE
    {
    paint3D();
    }

  virtual void paint3D() = 0;

private:
  Renderer *_renderer;
  ScreenFrameBuffer *_buffer;
  };

}

#endif

namespace Eks
{

class Canvas3D : public
#if X_ENABLE_DX_RENDERER
    D3D3DCanvas
#else
    GL3DCanvas
#endif X_ENABLE_GL_RENDERER
    , public AbstractCanvas
  {
public:
  X_CANVAS_GENERAL_MOUSEHANDLERS()

  virtual void update(AbstractRenderModel::UpdateMode);
  };

}

#endif // X3DCANVAS_H
