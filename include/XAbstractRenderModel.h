#ifndef XABSTRACTRENDERMODEL_H
#define XABSTRACTRENDERMODEL_H

#include "X3DGlobal.h"
#include "XVector"
#include "XProperty"

namespace Eks
{

class AbstractCanvas;
class AbstractDelegate;

class EKS3D_EXPORT AbstractRenderModel
  {
XProperties:
  XRORefProperty(Vector<AbstractCanvas*>, canvases);

public:
  class EKS3D_EXPORT Iterator
    {
  public:
    virtual ~Iterator();
    virtual bool next() = 0;
    };

  enum UpdateMode
    {
    RenderChange = 1<<0,
    BoundsChange = 1<<1 | RenderChange,
    TreeChange = 1<<2 | BoundsChange | RenderChange
    };

  virtual ~AbstractRenderModel();

  void update(UpdateMode m) const;

  virtual Iterator *createIterator() const = 0;

  virtual void resetIterator(Iterator *) const = 0;

  virtual const AbstractDelegate *delegateFor(Iterator *, const AbstractCanvas *) const = 0;

private:
  friend class AbstractCanvas;
  };

}

#endif // XABSTRACTRENDERMODEL_H
