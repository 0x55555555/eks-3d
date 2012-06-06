#ifndef XABSTRACTDELEGATE_H
#define XABSTRACTDELEGATE_H

#include "X3DGlobal.h"
#include "XAbstractRenderModel.h"

class XAbstractCanvas;

class EKS3D_EXPORT XAbstractDelegate
  {
public:
  virtual ~XAbstractDelegate();

  virtual void update(const XAbstractCanvas *, const XAbstractRenderModel::Iterator *, const XAbstractRenderModel *) const { };
  virtual void paint(xuint32 pass, XAbstractCanvas *, const XAbstractRenderModel::Iterator *, const XAbstractRenderModel *) const = 0;
  virtual xuint32 maxNumberOfPasses(XAbstractCanvas *, const XAbstractRenderModel::Iterator *, const XAbstractRenderModel *) const { return 1; }
  };

#endif // XABSTRACTDELEGATE_H
