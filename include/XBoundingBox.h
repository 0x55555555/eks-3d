#ifndef Cuboid_H
#define Cuboid_H

#include "X3DGlobal.h"
#include "Math/XMathVector.h"
#include "Utilities/XProperty.h"
#include "Math/XFrame.h"
#include "XTransform.h"

namespace Eks
{

class Line;

class EKS3D_EXPORT BoundingBox
  {
public:
  XRORefProperty( Vector3D, minimum );
  XRORefProperty( Vector3D, maximum );
  XROProperty( bool, isValid )

public:

  BoundingBox( );
  BoundingBox( Vector3D minimum, Vector3D maximum );

  void clear();

  bool operator==(const BoundingBox& oth) const;
  bool operator!=(const BoundingBox& oth) const;

  Vector3D centre() const;

  void unite( const Vector3D & );
  void unite( const BoundingBox & );

  BoundingBox united( const Vector3D & ) const;
  BoundingBox united( const BoundingBox & ) const;

  BoundingBox &operator|=( const BoundingBox & );
  BoundingBox operator|( const BoundingBox & ) const;

  void setMinimum( const Vector3D & );
  void setMaximum( const Vector3D & );

  void expand(float amount);

  Vector3D size() const;

  Real maximumDistanceSquared() const;

  void buildExtremities(Vector3D (&pts)[8]) const;

  bool contains( const Vector3D & ) const;
  bool contains( const BoundingBox & ) const;
  bool intersects( const BoundingBox & ) const;
  bool intersects( const Line &, float &t ) const;

  friend EKS3D_EXPORT std::ostream &operator<<(std::ostream &stream, const BoundingBox &itemRequest);
  friend EKS3D_EXPORT std::istream &operator>>(std::istream &stream, BoundingBox &itemRequest);
  };

class EKS3D_EXPORT AxisAlignedBoundingBox
  {
XProperties:
  XByRefProperty(Frame, frame, setFrame);
  XByRefProperty(BoundingBox, bounds, setBounds);

public:
  AxisAlignedBoundingBox();
  AxisAlignedBoundingBox(const Frame &frame, const BoundingBox &bnds = BoundingBox());

  void buildExtremities(Vector3D (&pts)[8]) const;

  void maximumExtents(const Eks::Vector3D &, float &min, float &max) const;

  void expandAligned(const Eks::BoundingBox &bnds);
  };
}

inline Eks::BoundingBox operator *( const Eks::Transform &mat, const Eks::BoundingBox &cub )
  {
  if(!cub.isValid())
    {
    return cub;
    }

  Eks::Vector3D pts[8];
  cub.buildExtremities(pts);

  Eks::BoundingBox ret;
  xForeach(const auto &pt, pts)
    {
    ret.unite(mat * pt);
    }
  return ret;
  }


#if X_QT_INTEROP

Q_DECLARE_METATYPE(Eks::BoundingBox)

#endif

#endif // Cuboid_H
