#ifndef XABSTRACTCANVAS_H
#define XABSTRACTCANVAS_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XAbstractRenderModel.h"

class XAbstractCanvasController;

class EKS3D_EXPORT XAbstractCanvas
  {
XProperties:
  XROProperty(XAbstractRenderModel *, model);
  XProperty(XAbstractCanvasController *, controller, setController);

public:
  XAbstractCanvas(XAbstractRenderModel *model=0, XAbstractCanvasController *controller=0);
  virtual ~XAbstractCanvas();

  virtual void update(XAbstractRenderModel::UpdateMode);
  virtual void paint();

  virtual bool isShown() = 0;

  void setModel(XAbstractRenderModel *);

private:
  void doUpdate();
  XAbstractRenderModel::Iterator *_iterator;
  bool _delayedUpdate;
  };

#endif // XABSTRACTCANVAS_H
