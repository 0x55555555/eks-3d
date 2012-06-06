#include "XPlane.h"
#include "XLine.h"

XPlane::XPlane( ) : _d(0.0f)
  {
  }

XPlane::XPlane( float a, float b, float c, float d )
  {
  setD(d);
  setNormal(XVector3D(a,b,c));
  }

XPlane::XPlane( const XVector3D &point, const XVector3D &normal )
  {
  set(point, normal);
  }

XPlane::XPlane( const XVector3D &point, const XVector3D &liesOnPlaneA, const XVector3D &liesOnPlaneB )
  {
  XVector3D c = (liesOnPlaneA - point).cross(liesOnPlaneB - point);
  if(c.dot(point) < 0)
    {
    c *= -1;
    }
  set( point, c );
  }

XVector3D XPlane::position() const
  {
  return normal() * d();
  }

float XPlane::a() const
  {
  return _normal.x();
  }

float XPlane::b() const
  {
  return _normal.y();
  }

float XPlane::c() const
  {
  return _normal.z();
  }

void XPlane::set( const XVector3D &point, const XVector3D &n )
  {
  setNormal(n);
  setD(point.dot(normal()));
  }

void XPlane::setNormal( const XVector3D &normal )
  {
  _normal = normal.normalized( );
  }

float XPlane::distanceToPlane( const XVector3D &in ) const
  {
  return in.dot(normal()) + d();
  }

float XPlane::intersection( const XLine &a ) const
  {
  xReal denominator = normal().dot(a.direction());
  if( !qFuzzyCompare( denominator, 0.0f ) )
    {
    return normal().dot(position() - a.position()) / denominator;
    }
  return HUGE_VAL;
  }

void XPlane::transform(const XTransform &tx)
  {
  XVector3D p = tx * position();
  XVector3D n = tx.linear() * normal();

  *this = XPlane(p, n);
  }
