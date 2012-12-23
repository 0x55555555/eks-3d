#ifndef XTRIANGLE_H
#define XTRIANGLE_H

#include "XProperty"
#include "XMathVector"

namespace Eks
{

class Line;

class Triangle
  {
  XProperty( Vector3D, pointA, setPointA );
  XProperty( Vector3D, pointB, setPointB );
  XProperty( Vector3D, pointC, setPointC );

public:
  Triangle( const Vector3D &pA, const Vector3D &pB, const Vector3D &pC );

  // does the given point lie on the triangle
  bool liesOn( const Vector3D &point ) const;

  bool intersects( const Line &ray, Vector3D &pos );
  };

}

#endif // XTRIANGLE_H
