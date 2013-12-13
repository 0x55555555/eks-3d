#ifndef Plane_H
#define Plane_H

#include "X3DGlobal.h"
#include "Utilities/XProperty.h"
#include "Math/XMathVector.h"
#include "XTransform.h"

namespace Eks
{

template <xsize Num> class LineBase;

class EKS3D_EXPORT Plane
  {
  XROProperty(Vector3D, normal);
  XProperty(float, d, setD);

public:
  Plane( );
  Plane( float a, float b, float c, float d );
  Plane( const Vector3D &point, const Vector3D &normal );
  Plane( const Vector3D &point, const Vector3D &liesOnPlaneA, const Vector3D &liesOnPlaneB );

  Vector3D position() const;

  void set( const Vector3D &point, const Vector3D &normal );
  void setNormal(const Vector3D & );

  float distanceToPlane( const Vector3D & ) const;
  float intersection( const LineBase<3> & ) const;

  void transform( const Transform &tx );

  float a() const;
  float b() const;
  float c() const;
  };

}

#endif // Plane_H
