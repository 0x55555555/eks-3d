#ifndef X3DCANVAS_H
#define X3DCANVAS_H

#include "X3DGlobal.h"
#include "XAbstractCanvas.h"
#include "XAbstractCanvasController.h"

#if X_QT_INTEROP

#include "QtGui/QMouseEvent"

namespace Eks
{
class ScreenFrameBuffer;
class Renderer;
class FrameBuffer;
}

#if X_ENABLE_GL_RENDERER

#include "QtOpenGL/QGLWidget"

namespace Eks
{

class EKS3D_EXPORT GL3DCanvas : public QGLWidget
  {
  Q_OBJECT

public:
  GL3DCanvas(QWidget *parent=0);
  ~GL3DCanvas();

  void resizeGL(int w, int h) X_OVERRIDE;
  void initializeGL() X_OVERRIDE;
  void paintGL() X_OVERRIDE;

signals:
  void initialise3D(Eks::Renderer *r);
  void paint3D(Eks::Renderer *r, Eks::FrameBuffer *buffer);
  void resize3D(Eks::Renderer *r, xuint32 w, xuint32 h);

public slots:
  void update3D();

private:
  Renderer *_renderer;
  ScreenFrameBuffer *_buffer;
  };

}

#endif

#if X_ENABLE_DX_RENDERER

#include "QtWidgets/QWidget"

class QPaintEngine;

namespace Eks
{

class EKS3D_EXPORT D3D3DCanvas : public QWidget
  {
  Q_OBJECT

public:
  D3D3DCanvas(QWidget* parent = 0, Eks::Renderer **r = 0);
  ~D3D3DCanvas();

  QPaintEngine* paintEngine() const X_OVERRIDE
    {
    return 0;
    }

  Renderer *renderer() { return _renderer; }
  ScreenFrameBuffer *buffer() { return _buffer; }

public slots:
  void update3D();

signals:
  void initialise3D(Eks::Renderer *r);
  void paint3D(Eks::Renderer *r, Eks::FrameBuffer *buffer);
  void resize3D(Eks::Renderer *r, xuint32 w, xuint32 h);

protected:
  void resizeEvent(QResizeEvent* evt) X_OVERRIDE;
  void paintEvent(QPaintEvent*) X_OVERRIDE;

private:
  Renderer *_renderer;
  ScreenFrameBuffer *_buffer;
  };

}

#endif

namespace Eks
{

class EKS3D_EXPORT Canvas3D : public AbstractCanvas
  {
public:
  static QWidget* createBest(QWidget *parent);
  };

}

#endif

#endif // X3DCANVAS_H
