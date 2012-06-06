#ifndef XCUBOID_H
#define XCUBOID_H

#include "X3DGlobal.h"
#include "XVector3D"
#include "XSize"
#include "XProperty"
#include "XTransform.h"

class XLine;

class EKS3D_EXPORT XCuboid
    {
public:
    XROProperty( XVector3D, minimum );
    XROProperty( XVector3D, maximum );
    XROProperty( bool, isValid )

public:
    XCuboid( );
    XCuboid( XVector3D minimum, XVector3D maximum );
    XCuboid( XVector3D minimum, XSize );

    bool operator==(const XCuboid& oth) const;
    bool operator!=(const XCuboid& oth) const;

    XVector3D centre() const;

    void unite( const XVector3D & );
    void unite( const XCuboid & );

    XCuboid united( const XVector3D & ) const;
    XCuboid united( const XCuboid & ) const;

    XCuboid &operator|=( const XCuboid & );
    XCuboid operator|( const XCuboid & ) const;

    void setMinimum( const XVector3D & );
    void setMaximum( const XVector3D & );

    void expand(float amount);

    XSize size() const;

    xReal maximumDistanceSquared() const;

    bool isInside( const XVector3D & ) const;
    bool intersects( const XCuboid & ) const;
    bool intersects( const XLine &, float &distance ) const;

    friend EKS3D_EXPORT QDataStream &operator<<(QDataStream &stream, const XCuboid &itemRequest);
    friend EKS3D_EXPORT QDataStream &operator>>(QDataStream &stream, XCuboid &itemRequest);
    friend EKS3D_EXPORT QTextStream &operator<<(QTextStream &stream, const XCuboid &itemRequest);
    friend EKS3D_EXPORT QTextStream &operator>>(QTextStream &stream, XCuboid &itemRequest);
    };

Q_DECLARE_METATYPE(XCuboid)

inline XCuboid operator *( const XTransform &mat, const XCuboid &cub )
  {
  XVector3D min = mat * cub.minimum();
  XVector3D max = mat * cub.maximum();

  XCuboid ret;
  ret.unite(min);
  ret.unite(max);
  return ret;
  }

#endif // XCUBOID_H
