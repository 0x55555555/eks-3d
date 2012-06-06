#include "X3DCanvas.h"

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
