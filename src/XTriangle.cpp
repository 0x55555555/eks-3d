#include "XTriangle.h"
#include "XLine.h"
#include "XPlane.h"
#include "QDebug"

namespace Eks
{

Triangle::Triangle( const Vector3D &pA, const Vector3D &pB, const Vector3D &pC )
  : _pointA(pA), _pointB(pB), _pointC(pC)
  {
  }

bool Triangle::intersects( const Line &ray, Vector3D &pos )
  {
  Plane pla( pointA(), pointB(), pointC() );
  pos = ray.sample( pla.intersection( ray ) );
  return liesOn( pos );
  }

// http://blackpawn.com/texts/pointinpoly/default.html
bool Triangle::liesOn( const Vector3D &p ) const
  {
  Vector3D v0 = pointC() - pointA();
  Vector3D v1 = pointB() - pointA();
  Vector3D v2 = p - pointA();

  Real dot00 = v0.dot(v0);
  Real dot01 = v0.dot(v1);
  Real dot02 = v0.dot(v2);
  Real dot11 = v1.dot(v1);
  Real dot12 = v1.dot(v2);

  Real invDenominator = 1.0f / ( dot00 * dot11 - dot01 * dot01 );
  Real u = ( dot11 * dot02 - dot01 * dot12 ) * invDenominator;
  Real v = ( dot00 * dot12 - dot01 * dot02 ) * invDenominator;

  if( u != u || v != v )
    {
    return false;
    }

  return ( u >= 0.0f ) && ( v >= 0.0f ) && ( u + v <= 1.0f );
  }
}
