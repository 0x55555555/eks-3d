#include "X3DCanvas.h"

#if X_QT_INTEROP

#include "XCore"
#include "XOptional"
#include "XGLRenderer.h"
#include "XD3DRenderer.h"
#include "XFramebuffer.h"
#include <QtOpenGL>

namespace Eks
{

#ifdef X_GL_EXTERNAL_CONTEXT
#ifdef Q_OS_WIN32
class WinGLContext
  {
public:
  WinGLContext(WId id)
    {
    _hWnd = (HWND)id;

    // get the device context (DC)
    _hDC = GetDC( _hWnd );

    // set the pixel format for the DC
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                  PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int format = ChoosePixelFormat( _hDC, &pfd );
    SetPixelFormat( _hDC, format, &pfd );

    // create the render context (RC)
    _hRC = wglCreateContext( _hDC );
    }

  ~WinGLContext()
    {
    destroy();
    }

  void destroy()
    {
    if (_hRC)
      {
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(_hRC);
      }

    if (_hWnd && _hDC)
      {
      ReleaseDC(_hWnd, _hDC);
      }

    _hWnd = NULL;
    _hDC = NULL;
    _hRC = NULL;
    }

  void bind()
    {
    // make it the current render context
    wglMakeCurrent( _hDC, _hRC );
    }

  void begin()
    {
    BeginPaint(_hWnd, &_ps);
    }

  void end()
    {
    SwapBuffers(_hDC);
    EndPaint(_hWnd, &_ps);
    }

private:
  HWND _hWnd;
  HDC _hDC;
  HGLRC _hRC;

  PAINTSTRUCT _ps;
  };
#endif

#endif

#define ALLOC Eks::Core::defaultAllocator()

#if X_ENABLE_GL_RENDERER

#ifdef X_GL_EXTERNAL_CONTEXT

GL3DCanvas::GL3DCanvas(QWidget *parent) :
  QWidget(parent)
  {
  setAttribute(Qt::WA_PaintOnScreen, true);
  setAttribute(Qt::WA_NativeWindow, true);

  WId handle = winId();
  _context = ALLOC->create<X_GL_EXTERNAL_CONTEXT>(handle);

  _context->bind();
  _buffer = ALLOC->create<ScreenFrameBuffer>();
  _renderer = GLRenderer::createGLRenderer(_buffer, ALLOC);

  QTimer::singleShot(0, Qt::CoarseTimer, this, SLOT(doInitialise3D()));
  }

GL3DCanvas::~GL3DCanvas()
  {
  xAssert(_context);
  _context->bind();

  Eks::GLRenderer::destroyGLRenderer(_renderer, _buffer, ALLOC);
  ALLOC->destroy(_buffer);
  _buffer = 0;

  ALLOC->destroy(_context);
  }

void GL3DCanvas::resizeEvent(QResizeEvent* evt)
  {
  xAssert(_context);
  _context->bind();

  _buffer->resize(evt->size().width(), evt->size().height(), ScreenFrameBuffer::RotateNone);
  emit resize3D(_renderer, evt->size().width(), evt->size().height());
  }

void GL3DCanvas::paintEvent(QPaintEvent *)
  {
  xAssert(_context);
  _context->begin();

  emit paint3D(_renderer, _buffer);

  bool deviceLost = false;
  _buffer->present(&deviceLost);
  xAssert(!deviceLost);

  _context->end();
  }

void GL3DCanvas::doInitialise3D()
  {
  emit initialise3D(_renderer);
  }

#else

GL3DCanvas::GL3DCanvas(QWidget *parent) :
  QGLWidget(parent)
  {
  _buffer = 0;
  _renderer = 0;
  }

GL3DCanvas::~GL3DCanvas()
  {
  Eks::GLRenderer::destroyGLRenderer(_renderer, _buffer, ALLOC);
  ALLOC->destroy(_buffer);
  _buffer = 0;
  }

void GL3DCanvas::paintGL()
  {
  Q_EMIT paint3D(_renderer, _buffer);
  }

void GL3DCanvas::initializeGL()
  {
  _buffer = ALLOC->create<ScreenFrameBuffer>();

  _renderer = GLRenderer::createGLRenderer(_buffer, ALLOC);
  Q_EMIT initialise3D(_renderer);
  }

void GL3DCanvas::resizeGL(int w, int h)
  {
  Q_EMIT resize3D(_renderer, w, h);
  }

#endif

void GL3DCanvas::update3D()
  {
  update();
  }

#endif

#if X_ENABLE_DX_RENDERER


D3D3DCanvas::D3D3DCanvas(QWidget* parent, Renderer **r)
    : QWidget(parent)
  {
  setAttribute(Qt::WA_PaintOnScreen, true);
  setAttribute(Qt::WA_NativeWindow, true);

  WId handle = winId();

  Eks::Optional<Renderer *> renderer(r);

  _buffer = ALLOC->create<ScreenFrameBuffer>();
  _renderer = renderer = Eks::D3DRenderer::createD3DRenderer((void*)handle, _buffer, ALLOC);

  QTimer::singleShot(0, Qt::CoarseTimer, this, SLOT(doInitialise3D()));
  }

D3D3DCanvas::~D3D3DCanvas()
  {
  Eks::D3DRenderer::destroyD3DRenderer(_renderer, _buffer, ALLOC);
  ALLOC->destroy(_buffer);
  _buffer = 0;
  }

void D3D3DCanvas::update3D()
  {
  update();
  }

void D3D3DCanvas::resizeEvent(QResizeEvent* evt)
  {
  _buffer->resize(evt->size().width(), evt->size().height(), ScreenFrameBuffer::RotateNone);
  resize3D(_renderer, evt->size().width(), evt->size().height());
  }

void D3D3DCanvas::paintEvent(QPaintEvent *)
  {
  paint3D(_renderer, _buffer);

  bool deviceLost = false;
  _buffer->present(&deviceLost);
  xAssert(!deviceLost);
  }

void D3D3DCanvas::doInitialise3D()
  {
  emit initialise3D(_renderer);
  }

#endif

QWidget* Canvas3D::createBest(QWidget* parent, AbstractCanvas **canvasOut)
  {

#if X_ENABLE_DX_RENDERER
  if(false && QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS8)
    {
    Renderer *ren = 0;
    D3D3DCanvas *can = new D3D3DCanvas(parent, &ren);
    *canvasOut = can;
    return can;
    }
#endif

#if X_ENABLE_GL_RENDERER
  GL3DCanvas *can = new GL3DCanvas(parent);
  *canvasOut = can;
  return can;
#else
  return 0;
#endif
  }
}

#endif
