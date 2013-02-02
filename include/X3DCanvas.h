#ifndef X3DCANVAS_H
#define X3DCANVAS_H

#include "X3DGlobal.h"
#include "XAbstractCanvas.h"
#include "XAbstractCanvasController.h"

#if X_QT_INTEROP

#include "QtGui/QMouseEvent"

namespace Eks
{
class FrameBuffer;
}

#if X_ENABLE_GL_RENDERER

#include "QtOpenGL/QGLWidget"

namespace Eks
{

class EKS3D_EXPORT GL3DCanvas : public QGLWidget
  {
public:
  GL3DCanvas(QWidget *parent=0);

  virtual void paintGL() X_OVERRIDE
    {
    paint3D();
    }
  virtual void paint3D() = 0;

  virtual bool isShown();
  };

}

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

  Renderer *renderer() { return _renderer; }
  ScreenFrameBuffer *buffer() { return _buffer; }

protected:
  void resizeEvent(QResizeEvent* evt) X_OVERRIDE;
  void paintEvent(QPaintEvent*) X_OVERRIDE;

  virtual void initialise3D(Eks::Renderer *r) = 0;
  virtual void paint3D(Eks::Renderer *r, Eks::FrameBuffer *buffer) = 0;
  virtual void resize3D(Eks::Renderer *r, xuint32 w, xuint32 h) = 0;

private:
  Renderer *_renderer;
  ScreenFrameBuffer *_buffer;
  };

}

#endif

namespace Eks
{

class EKS3D_EXPORT Canvas3D : public
#if X_ENABLE_DX_RENDERER
    D3D3DCanvas
#elif X_ENABLE_GL_RENDERER
    GL3DCanvas
#endif
    , public AbstractCanvas
  {
  Q_OBJECT

public:
  typedef
#if X_ENABLE_DX_RENDERER
      D3D3DCanvas
#elif X_ENABLE_GL_RENDERER
      GL3DCanvas
#endif
    Base;

  Canvas3D(QWidget *w);

  X_CANVAS_GENERAL_MOUSEHANDLERS()

  virtual void update(AbstractRenderModel::UpdateMode);

  bool isShown() X_OVERRIDE;

public slots:
  void update3D();
  };

}

#endif

#endif // X3DCANVAS_H
