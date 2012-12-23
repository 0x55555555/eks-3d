#include "XFrustum.h"
#include "XCuboid.h"
#include "XPlane.h"

namespace Eks
{

Frustum::Frustum( )
  {
  }

Frustum::Frustum( const Vector3D &point, const Vector3D &look, const Vector3D &across,
    const Vector3D &up, float viewAngle, float aspect, float nearPlane, float farPlane )
  {
  Vector3D lookNorm = look.normalized();
  Vector3D acrossNorm = across.normalized();
  Vector3D upNorm = up.normalized();
  float fovUpY = tan(X_DEGTORAD(viewAngle)/2.0f);
  float fovUpX = tan(X_DEGTORAD(viewAngle*aspect)/2.0f);

  // near plane
  _planes[NearPlane] = Plane(point+(lookNorm*nearPlane), -lookNorm);
  // far plane
  _planes[FarPlane] = Plane(point+(lookNorm*farPlane), lookNorm);

  // top plane
  _planes[TopPlane] = Plane(point, (lookNorm + (fovUpY * upNorm)).cross(across) );
  // bottom plane
  _planes[BottomPlane] = Plane(point, across.cross(lookNorm - (fovUpY * upNorm)) );

  // left plane
  _planes[LeftPlane] = Plane(point, up.cross(lookNorm+(fovUpX * acrossNorm)) );
  // right plane
  _planes[RightPlane] = Plane(point, (lookNorm - (fovUpX * acrossNorm)).cross(up) );

  }

Frustum::IntersectionResult Frustum::intersects( const Cuboid &cuboid ) const
  {
  Frustum::IntersectionResult ret = Inside;
  Vector3D vec;
  for(xsize i=0; i<6; ++i)
    {
    vec.x() = (_planes[i].normal().x() >= 0.0f) ? cuboid.maximum().x() : cuboid.minimum().x();
    vec.y() = (_planes[i].normal().y() >= 0.0f) ? cuboid.maximum().y() : cuboid.minimum().y();
    vec.z() = (_planes[i].normal().z() >= 0.0f) ? cuboid.maximum().z() : cuboid.minimum().z();

    if( _planes[i].distanceToPlane(vec) < 0.0f )
      {
      return Outside;
      }

    vec.x() = (_planes[i].normal().x() >= 0.0f) ? cuboid.minimum().x() : cuboid.maximum().x();
    vec.y() = (_planes[i].normal().y() >= 0.0f) ? cuboid.minimum().y() : cuboid.maximum().y();
    vec.z() = (_planes[i].normal().z() >= 0.0f) ? cuboid.minimum().z() : cuboid.maximum().z();

    if( _planes[i].distanceToPlane(vec) < 0.0f )
      {
      ret = Intersects;
      }
    }
  return ret;
  }

void Frustum::transform( const Transform &tx )
  {
  for(xsize i=0; i<6; ++i)
    {
    _planes[i].transform(tx);
    }
  }

}
