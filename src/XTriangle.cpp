#include "XTriangle.h"
#include "XLine.h"
#include "XPlane.h"
#include "QDebug"

XTriangle::XTriangle( const XVector3D &pA, const XVector3D &pB, const XVector3D &pC )
    : _pointA(pA), _pointB(pB), _pointC(pC)
  {
  }

bool XTriangle::intersects( const XLine &ray, XVector3D &pos )
  {
  XPlane pla( pointA(), pointB(), pointC() );
  pos = ray.sample( pla.intersection( ray ) );
  return liesOn( pos );
  }

// http://blackpawn.com/texts/pointinpoly/default.html
bool XTriangle::liesOn( const XVector3D &p ) const
    {
    XVector3D v0 = pointC() - pointA();
    XVector3D v1 = pointB() - pointA();
    XVector3D v2 = p - pointA();

    xReal dot00 = v0.dot(v0);
    xReal dot01 = v0.dot(v1);
    xReal dot02 = v0.dot(v2);
    xReal dot11 = v1.dot(v1);
    xReal dot12 = v1.dot(v2);

    xReal invDenominator = 1.0f / ( dot00 * dot11 - dot01 * dot01 );
    xReal u = ( dot11 * dot02 - dot01 * dot12 ) * invDenominator;
    xReal v = ( dot00 * dot12 - dot01 * dot02 ) * invDenominator;

    if( u != u || v != v )
        {
        return false;
        }

    return ( u >= 0.0f ) && ( v >= 0.0f ) && ( u + v <= 1.0f );
    }
