#ifndef XSHAPE_H
#define XSHAPE_H

#include "X3DGlobal.h"
#include "Containers/XVariant.h"
#include "XLine.h"
#include "XPlane.h"

namespace Eks
{


class Shape
  {
public:
  typedef Variant<Plane, Line> ShapeVariant;

  template <typename T, typename... Args> static Shape create(Args&&... args)
    {
    Shape s;
    s.set<T>(std::forward<Args>(args)...);
    return s;
    }

  template <typename T, typename... Args> void set(Args&&... args)
    {
    _data.set<T>(std::forward<Args>(args)...);
    }

  template <typename T> bool is()
    {
    return _data.is<T>();
    }

  ShapeVariant::VariantType type() const
    {
    return _data.type();
    }

  template <typename T> ShapeVariant::VariantType typeFor() const
    {
    return _data.typeFor<T>();
    }

  Shape intersect(const Shape& s);


private:
  ShapeVariant _data;
  };

}

#endif // XSHAPE_H
