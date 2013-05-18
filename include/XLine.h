#ifndef XLINE_H
#define XLINE_H

#include "X3DGlobal.h"
#include "XMathVector"
#include "XProperty"
#include "XTransform.h"

namespace Eks
{

class EKS3D_EXPORT Line
  {
  XByRefProperty( Vector3D, position, setPosition );
  XByRefProperty( Vector3D, direction, setDirection );
public:
  enum ConstructionMode
    {
    TwoPoints,
    PointAndDirection
    };

  Line( const Vector3D &, const Vector3D &, ConstructionMode=TwoPoints );

  void transform(const Eks::Transform &);

  Vector3D sample(float) const;

  // returns t for this line, for the closest point on that line
  float closestPointOn(const Line &l) const;

  // returns the closest t to l
  float closestPointTo(const Vector3D &l) const;

  Vector3D pointAtDistance(float distAlongLine) const;
  };

}

inline Eks::Line operator*( const Eks::Transform &tx, const Eks::Line &line )
  {
  Eks::Line l(line);
  l.transform(tx);
  return l;
  }


#endif // XLINE_H
