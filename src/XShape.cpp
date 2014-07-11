#include "XShape.h"

namespace Eks
{

Eks::Shape Eks::Shape::intersect(const Eks::Shape &s)
  {
  const Eks::Shape* a = this;
  const Eks::Shape* b = &s;
  if (b < a)
    {
    std::swap(a, b);
    }

  if (a->type() == ShapeVariant::VariantType::Null)
    {
    return Eks::Shape();
    }

  if (a->type() == ShapeVariant::typeFor<Plane>())
    {
    }

  return Eks::Shape();
  }

}
