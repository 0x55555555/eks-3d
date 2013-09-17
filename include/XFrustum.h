#ifndef XFRUSTUM
#define XFRUSTUM

#include "XGlobal"
#include "XTransform.h"
#include "XPlane.h"

namespace Eks
{

class BoundingBox;

class EKS3D_EXPORT Frustum
  {
public:
  enum IntersectionResult
    {
    Inside,
    Intersects,
    Outside
    };

  enum PlaneType
    {
    NearPlane,
    LeftPlane,
    RightPlane,
    BottomPlane,
    TopPlane,
    FarPlane
    };

  Frustum();
  Frustum( const Vector3D &point, const Vector3D &look, const Vector3D &across, const Vector3D &up,
             float angle, float aspect, float near, float far );

  IntersectionResult intersects(const BoundingBox &) const;

  void transform( const Transform &tx );

private:
  Plane _planes[6];
  };
}

#endif // XFRUSTRUM
