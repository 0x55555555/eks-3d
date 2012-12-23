#include "XLine.h"
#include "XTransform.h"
#include "XPlane.h"

namespace Eks
{

Line::Line( const Eks::Vector3D &p1, const Eks::Vector3D &p2, ConstructionMode mode ) : _position(p1)
  {
  if(mode == TwoPoints)
    {
    _direction = (p2-p1).normalized();
    }
  else
    {
    _direction = p2;
    }
  }

Eks::Vector3D Line::sample( float t ) const
  {
  return position() + ( t * direction() );
  }

void Line::transform( const Transform &tx )
  {
  _position = Eks::Vector3D(tx * _position);
  _direction = Eks::Vector3D(tx.linear() * _direction);
  }

float Line::closestPointOn(const Line &l) const
  {
  Eks::Vector3D lDirNorm = l.direction().normalized();
  Eks::Vector3D dirNorm = direction().normalized();

  Eks::Vector3D cP = lDirNorm.cross( lDirNorm.cross( dirNorm ) );
  if(cP.squaredNorm() > 0.001f)
    {
    return Plane( l.position(), cP ).intersection(*this);
    }

  return HUGE_VAL;
  }

float Line::closestPointTo(const Eks::Vector3D &l) const
  {
  Eks::Vector3D lineToPt = l - position();

  return direction().dot(lineToPt) / direction().squaredNorm();
  }

}
