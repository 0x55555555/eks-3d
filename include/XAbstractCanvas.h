#ifndef XABSTRACTCANVAS_H
#define XABSTRACTCANVAS_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XAbstractRenderModel.h"

namespace Eks
{

class AbstractCanvasController;

class EKS3D_EXPORT AbstractCanvas
  {
XProperties:
  XROProperty(AbstractRenderModel *, model);
  XProperty(AbstractCanvasController *, controller, setController);

public:
  AbstractCanvas(AbstractRenderModel *model=0, AbstractCanvasController *controller=0);
  virtual ~AbstractCanvas();

  virtual void update(AbstractRenderModel::UpdateMode);
  virtual void paint();

  virtual bool isShown() = 0;

  void setModel(AbstractRenderModel *);

private:
  void doUpdate();
  AbstractRenderModel::Iterator *_iterator;
  bool _delayedUpdate;
  };

}

#endif // XABSTRACTCANVAS_H
