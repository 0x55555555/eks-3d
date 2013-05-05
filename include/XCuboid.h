#ifndef Cuboid_H
#define Cuboid_H

#include "X3DGlobal.h"
#include "XMathVector"
#include "XProperty"
#include "XTransform.h"

namespace Eks
{

class Line;

class EKS3D_EXPORT Cuboid
  {
public:
  XRORefProperty( Vector3D, minimum );
  XRORefProperty( Vector3D, maximum );
  XROProperty( bool, isValid )

public:

  Cuboid( );
  Cuboid( Vector3D minimum, Vector3D maximum );

  void clear();

  bool operator==(const Cuboid& oth) const;
  bool operator!=(const Cuboid& oth) const;

  Vector3D centre() const;

  void unite( const Vector3D & );
  void unite( const Cuboid & );

  Cuboid united( const Vector3D & ) const;
  Cuboid united( const Cuboid & ) const;

  Cuboid &operator|=( const Cuboid & );
  Cuboid operator|( const Cuboid & ) const;

  void setMinimum( const Vector3D & );
  void setMaximum( const Vector3D & );

  void expand(float amount);

  Vector3D size() const;

  Real maximumDistanceSquared() const;

  bool contains( const Vector3D & ) const;
  bool contains( const Cuboid & ) const;
  bool intersects( const Cuboid & ) const;
  bool intersects( const Line &, float &distance ) const;

  friend EKS3D_EXPORT QTextStream &operator<<(QTextStream &stream, const Cuboid &itemRequest);
  friend EKS3D_EXPORT QTextStream &operator>>(QTextStream &stream, Cuboid &itemRequest);
  friend EKS3D_EXPORT QDataStream &operator<<(QDataStream &stream, const Cuboid &itemRequest);
  friend EKS3D_EXPORT QDataStream &operator>>(QDataStream &stream, Cuboid &itemRequest);
  };
}

inline Eks::Cuboid operator *( const Eks::Transform &mat, const Eks::Cuboid &cub )
  {
  Eks::Vector3D min = mat * cub.minimum();
  Eks::Vector3D max = mat * cub.maximum();

  Eks::Cuboid ret;
  ret.unite(min);
  ret.unite(max);
  return ret;
  }


#if X_QT_INTEROP

Q_DECLARE_METATYPE(Eks::Cuboid)

#endif

#endif // Cuboid_H
