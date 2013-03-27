#ifndef X3DCANVAS_H
#define X3DCANVAS_H

#include "X3DGlobal.h"
#include "XAbstractCanvas.h"
#include "XAbstractCanvasController.h"

#if X_QT_INTEROP

#include "QtHelper/XQWidget"
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

#ifdef Q_OS_WIN32
class WinGLContext;
# define X_GL_EXTERNAL_CONTEXT WinGLContext
#endif

#ifdef X_GL_EXTERNAL_CONTEXT

class EKS3D_EXPORT GL3DCanvas
    : public QWidget,
      public AbstractCanvas
  {
  Q_OBJECT

public:
  GL3DCanvas(QWidget *parent=0);
  ~GL3DCanvas();

  X_CANVAS_GENERAL_MOUSEHANDLERS()

  QPaintEngine* paintEngine() const X_OVERRIDE { return 0; }

signals:
  void initialise3D(Eks::Renderer *r);
  void paint3D(Eks::Renderer *r, Eks::FrameBuffer *buffer);
  void resize3D(Eks::Renderer *r, xuint32 w, xuint32 h);

public slots:
  void update3D();

protected slots:
  void doInitialise3D();

private:
  void resizeEvent(QResizeEvent* evt) X_OVERRIDE;
  void paintEvent(QPaintEvent*) X_OVERRIDE;

  Renderer *_renderer;
  ScreenFrameBuffer *_buffer;

  X_GL_EXTERNAL_CONTEXT *_context;
  };

#else

class EKS3D_EXPORT GL3DCanvas
    : public QGLWidget
  {
  Q_OBJECT

public:
  GL3DCanvas(QWidget *parent=0);
  ~GL3DCanvas();

  X_CANVAS_GENERAL_MOUSEHANDLERS()

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

#endif

}

#endif

#if X_ENABLE_DX_RENDERER

class QPaintEngine;

namespace Eks
{

class EKS3D_EXPORT D3D3DCanvas
    : public QWidget,
      public AbstractCanvas
  {
  Q_OBJECT

public:
  D3D3DCanvas(QWidget* parent = 0, Eks::Renderer **r = 0);
  ~D3D3DCanvas();

  X_CANVAS_GENERAL_MOUSEHANDLERS()

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

protected slots:
  void doInitialise3D();

private:
  Renderer *_renderer;
  ScreenFrameBuffer *_buffer;
  };

}

#endif

namespace Eks
{

class EKS3D_EXPORT Canvas3D
  {
public:
  static QWidget* createBest(QWidget *parent, AbstractCanvas **canvasOut);
  };

}

#endif

#endif // X3DCANVAS_H
