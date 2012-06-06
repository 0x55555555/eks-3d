#ifndef X2DCANVAS_H
#define X2DCANVAS_H

#include "X3DGlobal.h"
#include "QWidget"
#include "XAbstractCanvas.h"
#include "XCameraCanvasController.h"
#include "QMouseEvent"


class QPainter;

class EKS3D_EXPORT X2DCanvas : public QWidget, public XAbstractCanvas
  {
  Q_OBJECT

XProperties:
  XProperty(QColor, backgroundColour, setBackgroundColour);
  XROProperty(QPainter *, currentPainter);
  XROProperty(QRect, region);
  XProperty(bool, antiAliasingEnabled, setAntiAliasingEnabled);
  XRefProperty(QTransform, transform);

public:
  X2DCanvas(QWidget *parent=0);

  X_CANVAS_GENERAL_MOUSEHANDLERS()

  virtual void update(XAbstractRenderModel::UpdateMode);

  virtual bool isShown();

protected:
  virtual void paintEvent(QPaintEvent *);
  };

class EKS3D_EXPORT XSimple2DCanvasController : public XCameraCanvasController, XCameraCanvasController::CameraInterface
  {
public:
  XSimple2DCanvasController(X2DCanvas *canvas);

  XCameraCanvasController::CameraInterface *camera() { return this; }


  virtual MovementFlags supportedMovements() const;
  virtual void zoom(float factor, float x, float y);
  virtual void track(float x, float y);
  virtual void dolly(float x, float y);
  virtual void pan(float x, float y);
  };

#endif // X2DCANVAS_H
