#include "XPlane.h"
#include "XLine.h"

namespace Eks
{

Plane::Plane( ) : _d(0.0f)
  {
  }

Plane::Plane( float a, float b, float c, float d )
  {
  setD(d);
  setNormal(Vector3D(a,b,c));
  }

Plane::Plane( const Vector3D &point, const Vector3D &normal )
  {
  set(point, normal);
  }

Plane::Plane( const Vector3D &point, const Vector3D &liesOnPlaneA, const Vector3D &liesOnPlaneB )
  {
  Vector3D c = (liesOnPlaneA - point).cross(liesOnPlaneB - point);
  if(c.dot(point) < 0)
    {
    c *= -1;
    }
  set( point, c );
  }

Vector3D Plane::position() const
  {
  return normal() * d();
  }

float Plane::a() const
  {
  return _normal.x();
  }

float Plane::b() const
  {
  return _normal.y();
  }

float Plane::c() const
  {
  return _normal.z();
  }

void Plane::set( const Vector3D &point, const Vector3D &n )
  {
  setNormal(n);
  setD(point.dot(normal()));
  }

void Plane::setNormal( const Vector3D &normal )
  {
  _normal = normal.normalized( );
  }

float Plane::distanceToPlane( const Vector3D &in ) const
  {
  return in.dot(normal()) + d();
  }

float Plane::intersection( const LineBase<3> &a ) const
  {
  Real denominator = normal().dot(a.direction());
  if( !qFuzzyCompare( denominator, 0.0f ) )
    {
    return normal().dot(position() - a.position()) / denominator;
    }
  return HUGE_VAL;
  }

void Plane::transform(const Eks::Transform &tx)
  {
  Vector3D p = tx * position();
  Vector3D n = tx.linear() * normal();

  *this = Plane(p, n);
  }

}
