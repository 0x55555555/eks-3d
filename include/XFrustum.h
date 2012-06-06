#ifndef XFRUSTUM
#define XFRUSTUM

class XCuboid;

#include "XGlobal"
#include "XTransform.h"
#include "XPlane.h"

class EKS3D_EXPORT XFrustum
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

  XFrustum();
  XFrustum( const XVector3D &point, const XVector3D &look, const XVector3D &across, const XVector3D &up,
             float angle, float aspect, float near, float far );

  IntersectionResult intersects(const XCuboid &) const;

  void transform( const XTransform &tx );

private:
  XPlane _planes[6];
  };

#endif // XFRUSTRUM
