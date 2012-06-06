#include "X2DCanvas.h"
#include "QPainter"
#include "QPaintEvent"
#include "QApplication"
#include "XAbstractCanvasController.h"
#include "QDebug"

X2DCanvas::X2DCanvas(QWidget *parent) : QWidget(parent), _backgroundColour(QApplication::palette().color(QPalette::Window)), _currentPainter(0)
  {
  }

void X2DCanvas::paintEvent(QPaintEvent *event)
  {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, antiAliasingEnabled());
  _currentPainter = &painter;

  _region = _transform.inverted().mapRect(event->rect());
  painter.fillRect(event->rect(), _backgroundColour);

  painter.setTransform(_transform);

  paint();

  _currentPainter = 0;
  }

void X2DCanvas::update(XAbstractRenderModel::UpdateMode c)
  {
  XAbstractCanvas::update(c);
  QWidget::update();
  }

bool X2DCanvas::isShown()
  {
  return !isHidden() && hasFocus();
  }


XSimple2DCanvasController::XSimple2DCanvasController(X2DCanvas *canvas) : XCameraCanvasController(canvas)
  {
  }

XSimple2DCanvasController::MovementFlags XSimple2DCanvasController::supportedMovements() const
  {
  return Track | Zoom;
  }

void XSimple2DCanvasController::zoom(float factor, float x, float y)
  {
  QTransform& transform = static_cast<X2DCanvas*>(canvas())->transform();

  if((factor > 1.0f && transform.m11() < 5.0f) || (factor < 1.0f && transform.m11() > 0.1f))
    {
    QPoint pos(x, y);
    pos = transform.inverted().map(pos);

    transform.translate(pos.x(), pos.y());

    transform.scale(factor, factor);

    transform.translate(-pos.x(), -pos.y());
    }
  }

void XSimple2DCanvasController::track(float x, float y)
  {
  QTransform& transform = static_cast<X2DCanvas*>(canvas())->transform();

  x /= transform.m11();
  y /= transform.m11();

  transform.translate(x, y);
  }

void XSimple2DCanvasController::dolly(float x, float y)
  {
  qDebug() << "dolly" << x << y;
  }

void XSimple2DCanvasController::pan(float x, float y)
  {
  qDebug() << "pan" << x << y;
  }
