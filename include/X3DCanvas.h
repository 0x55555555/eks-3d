#ifndef X3DCANVAS_H
#define X3DCANVAS_H

#include "XAbstractCanvas.h"
#include "XAbstractCanvasController.h"
#include "QGLWidget"
#include "QMouseEvent"

class EKS3D_EXPORT X3DCanvas : public QGLWidget, public XAbstractCanvas
  {
public:
  X3DCanvas(QWidget *parent=0);

  X_CANVAS_GENERAL_MOUSEHANDLERS()

  virtual void update(XAbstractRenderModel::UpdateMode);

  virtual void paintGL();

  virtual bool isShown();
  };

#endif // X3DCANVAS_H
