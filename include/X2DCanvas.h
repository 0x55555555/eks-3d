#ifndef X2DCANVAS_H
#define X2DCANVAS_H

#include "X3DGlobal.h"
#include "XAbstractCanvas.h"
#include "XCameraCanvasController.h"

#if X_QT_INTEROP

#include "QtHelper/XQWidget"
#include "QtGui/QMouseEvent"


class QPainter;

class EKS3D_EXPORT X2DCanvas : public QWidget, public Eks::AbstractCanvas
  {
XProperties:
  XProperty(QColor, backgroundColour, setBackgroundColour);
  XROProperty(QPainter *, currentPainter);
  XROProperty(QRect, region);
  XProperty(bool, antiAliasingEnabled, setAntiAliasingEnabled);
  XRefProperty(QTransform, transform);

public:
  X2DCanvas(QWidget *parent=0);

  X_CANVAS_GENERAL_MOUSEHANDLERS()

  //virtual void update(Eks::AbstractRenderModel::UpdateMode);

  virtual bool isShown();

protected:
  virtual void paintEvent(QPaintEvent *);
  };

class EKS3D_EXPORT XSimple2DCanvasController : public Eks::CameraCanvasController, Eks::CameraCanvasController::CameraInterface
  {
public:
  XSimple2DCanvasController(X2DCanvas *canvas);

  Eks::CameraCanvasController::CameraInterface *camera() { return this; }


  virtual MovementFlags supportedMovements() const;
  virtual void zoom(float factor, float x, float y);
  virtual void track(float x, float y);
  virtual void dolly(float x, float y);
  virtual void pan(float x, float y);
  };

#endif

#endif // X2DCANVAS_H
