#ifndef XABSTRACTDELEGATE_H
#define XABSTRACTDELEGATE_H

#include "X3DGlobal.h"
#include "XAbstractRenderModel.h"

namespace Eks
{

class AbstractCanvas;

class EKS3D_EXPORT AbstractDelegate
  {
public:
  virtual ~AbstractDelegate() { }

  virtual void update(const AbstractCanvas *, const AbstractRenderModel::Iterator *, const AbstractRenderModel *) const { }
  virtual void paint(xuint32 pass, AbstractCanvas *, const AbstractRenderModel::Iterator *, const AbstractRenderModel *) const = 0;
  virtual xuint32 maxNumberOfPasses(AbstractCanvas *, const AbstractRenderModel::Iterator *, const AbstractRenderModel *) const { return 1; }
  };

}

#endif // XABSTRACTDELEGATE_H
