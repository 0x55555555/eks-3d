#include "XLine.h"
#include "XTransform.h"
#include "XPlane.h"

XLine::XLine( XVector3D p1, XVector3D p2, ConstructionMode mode ) : _position(p1)
  {
  if(mode == TwoPoints)
    {
    _direction = p2-p1;
    }
  else
    {
    _direction = p2;
    }
  }

XVector3D XLine::sample( float t ) const
  {
  return position() + ( t * direction() );
  }

void XLine::transform( const XTransform &tx )
  {
  _position = XVector3D(tx * _position);
  _direction = XVector3D(tx.linear() * _direction);
  }

float XLine::closestPointOn(const XLine &l) const
  {
  XVector3D lDirNorm = l.direction().normalized();
  XVector3D dirNorm = direction().normalized();

  XVector3D cP = lDirNorm.cross( lDirNorm.cross( dirNorm ) );
  if(cP.squaredNorm() > 0.001f)
    {
    return XPlane( l.position(), cP ).intersection(*this);
    }

  return HUGE_VAL;
  }

float XLine::closestPointTo(const XVector3D &l) const
  {
  XVector3D lineToPt = l - position();

  return direction().dot(lineToPt) / direction().squaredNorm();
  }
