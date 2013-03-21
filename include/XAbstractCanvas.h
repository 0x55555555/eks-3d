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
  XProperty(AbstractCanvasController *, controller, setController);

public:
  AbstractCanvas(AbstractCanvasController *controller=0);
  virtual ~AbstractCanvas();
  };

}

#endif // XABSTRACTCANVAS_H
