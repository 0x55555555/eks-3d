#include "XFrustum.h"
#include "XCuboid.h"
#include "XPlane.h"

XFrustum::XFrustum( )
  {
  }

XFrustum::XFrustum( const XVector3D &point, const XVector3D &look, const XVector3D &across,
    const XVector3D &up, float viewAngle, float aspect, float nearPlane, float farPlane )
  {
  XVector3D lookNorm = look.normalized();
  XVector3D acrossNorm = across.normalized();
  XVector3D upNorm = up.normalized();
  float fovUpY = tan(X_DEGTORAD(viewAngle)/2.0f);
  float fovUpX = tan(X_DEGTORAD(viewAngle*aspect)/2.0f);

  // near plane
  _planes[NearPlane] = XPlane(point+(lookNorm*nearPlane), -lookNorm);
  // far plane
  _planes[FarPlane] = XPlane(point+(lookNorm*farPlane), lookNorm);

  // top plane
  _planes[TopPlane] = XPlane(point, (lookNorm + (fovUpY * upNorm)).cross(across) );
  // bottom plane
  _planes[BottomPlane] = XPlane(point, across.cross(lookNorm - (fovUpY * upNorm)) );

  // left plane
  _planes[LeftPlane] = XPlane(point, up.cross(lookNorm+(fovUpX * acrossNorm)) );
  // right plane
  _planes[RightPlane] = XPlane(point, (lookNorm - (fovUpX * acrossNorm)).cross(up) );

  }

XFrustum::IntersectionResult XFrustum::intersects( const XCuboid &cuboid ) const
  {
  XFrustum::IntersectionResult ret = Inside;
  XVector3D vec;
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

void XFrustum::transform( const XTransform &tx )
  {
  for(xsize i=0; i<6; ++i)
    {
    _planes[i].transform(tx);
    }
  }
