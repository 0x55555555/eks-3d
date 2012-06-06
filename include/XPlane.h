#ifndef XPLANE_H
#define XPLANE_H

#include "X3DGlobal.h"
#include "XProperty"
#include "XVector3D"
#include "XTransform.h"

class XLine;

class EKS3D_EXPORT XPlane
  {
  XROProperty(XVector3D, normal);
  XProperty(float, d, setD);

public:
  XPlane( );
  XPlane( float a, float b, float c, float d );
  XPlane( const XVector3D &point, const XVector3D &normal );
  XPlane( const XVector3D &point, const XVector3D &liesOnPlaneA, const XVector3D &liesOnPlaneB );

  XVector3D position() const;

  void set( const XVector3D &point, const XVector3D &normal );
  void setNormal(const XVector3D & );

  float distanceToPlane( const XVector3D & ) const;
  float intersection( const XLine & ) const;

  void transform( const XTransform &tx );

  float a() const;
  float b() const;
  float c() const;
  };

#endif // XPLANE_H
