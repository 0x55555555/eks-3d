#ifndef XGEOMETRY_H
#define XGEOMETRY_H

#include "XMathVector"
#include "XPrivateImpl"

namespace Eks
{

class Renderer;

class Geometry : public PrivateImpl<sizeof(void*)>
  {
public:
  Geometry(Renderer *r=0, const void *data=0, xsize elementSize=0, xsize elementCount=0);
  ~Geometry();

  static bool delayedCreate(Geometry &ths, Renderer *r, const void *data, xsize size, xsize count);

private:
  Renderer *_renderer;
  };

class IndexGeometry: public PrivateImpl<sizeof(void*)>
  {
public:
  enum Type
    {
    Unsigned16,

    TypeCount
    };

  IndexGeometry(Renderer *r=0, Type type=Unsigned16, const void *data=0, xsize dataSize=0);
  ~IndexGeometry();

  static bool delayedCreate(
    IndexGeometry &ths,
    Renderer *r,
    Type type,
    const void *indexData,
    xsize indexCount);

private:
  Renderer *_renderer;
  };

}

#endif // XGEOMETRY_H
