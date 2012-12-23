#include "XGeometry.h"
#include "XRenderer.h"
#include "XTriangle.h"
#include "XCuboid.h"

namespace Eks
{

Geometry::Geometry(Renderer *r, const void *data, xsize elementSize, xsize elementCount)
    : _renderer(0)
  {
  if(data)
    {
    xAssert(elementSize && elementCount);
    delayedCreate(*this, r, data, elementSize, elementCount);
    }
  }

Geometry::~Geometry()
  {
  if(_renderer)
    {
    _renderer->destroyGeometry(this);
    }
  }

bool Geometry::delayedCreate(
    Geometry &ths,
    Renderer *r,
    const void *data,
    xsize elementSize,
    xsize elementCount)
  {
  ths._renderer = r;
  return r->createGeometry(&ths, data, elementSize, elementCount);
  }

IndexGeometry::IndexGeometry(Renderer *r, Type type, const void *data, xsize dataSize)
    : _renderer(0)
  {
  if(data)
    {
    xAssert(dataSize);
    delayedCreate(*this, r, type, data, dataSize);
    }
  }

IndexGeometry::~IndexGeometry()
  {
  if(_renderer)
    {
    _renderer->destroyIndexGeometry(this);
    }
  }

bool IndexGeometry::delayedCreate(
    IndexGeometry &ths,
    Renderer *r,
    Type type,
    const void *index,
    xsize indexCount)
  {
  ths._renderer = r;
  return r->createIndexGeometry(&ths, type, index, indexCount);
  }

}

