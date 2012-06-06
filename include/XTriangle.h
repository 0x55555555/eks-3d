#ifndef XTRIANGLE_H
#define XTRIANGLE_H

#include "XProperty"
#include "XVector3D"

class XLine;

class XTriangle
  {
  XProperty( XVector3D, pointA, setPointA );
  XProperty( XVector3D, pointB, setPointB );
  XProperty( XVector3D, pointC, setPointC );

public:
  XTriangle( const XVector3D &pA, const XVector3D &pB, const XVector3D &pC );

  // does the given point lie on the triangle
  bool liesOn( const XVector3D &point ) const;

  bool intersects( const XLine &ray, XVector3D &pos );
  };

#endif // XTRIANGLE_H
