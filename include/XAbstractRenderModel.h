#ifndef XABSTRACTRENDERMODEL_H
#define XABSTRACTRENDERMODEL_H

#include "X3DGlobal.h"
#include "XList"
#include "XProperty"

class XAbstractCanvas;
class XAbstractDelegate;

class EKS3D_EXPORT XAbstractRenderModel
  {
XProperties:
  XRORefProperty(XList<XAbstractCanvas*>, canvases);

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

  virtual ~XAbstractRenderModel();

  void update(UpdateMode m) const;

  virtual Iterator *createIterator() const = 0;

  virtual void resetIterator(Iterator *) const = 0;

  virtual const XAbstractDelegate *delegateFor(Iterator *, const XAbstractCanvas *) const = 0;

private:
  friend class XAbstractCanvas;
  };

#endif // XABSTRACTRENDERMODEL_H
