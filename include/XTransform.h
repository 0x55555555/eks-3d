#ifndef XTRANSFORM_H
#define XTRANSFORM_H

#include "X3DGlobal.h"
#include "XList"
#include "Eigen/Geometry"
#include "XVector3D"
#include "XMatrix4x4"

typedef Eigen::Affine3f XTransform;
typedef Eigen::Projective3f XComplexTransform;

Q_DECLARE_METATYPE(XTransform)
Q_DECLARE_METATYPE(XComplexTransform)

namespace XTransformUtilities
{
XTransform EKS3D_EXPORT lookAt(const XVector3D &eye, const XVector3D &aim, const XVector3D &up);
XComplexTransform EKS3D_EXPORT perspective(xReal angle, xReal aspect, xReal nearPlane, xReal farPlane);
}

template <typename A, int B, int C, int D> QDebug operator <<(QDebug str, const Eigen::Transform <A, B, C, D> &data)
  {
  return str << data.matrix();
  }

template <typename A, int B, int C, int D> QTextStream &operator <<(QTextStream &str, const Eigen::Transform <A, B, C, D> &data)
  {
  return str << data.matrix();
  }

template <typename A, int B, int C, int D> QTextStream &operator >>(QTextStream &str, Eigen::Transform <A, B, C, D> &data)
  {
  return str >> data.matrix();
  }


template <typename A, int B, int C, int D> QDataStream &operator <<(QDataStream &str, const Eigen::Transform <A, B, C, D> &data)
  {
  return str << data.matrix();
  }

template <typename A, int B, int C, int D> QDataStream &operator >>(QDataStream &str, Eigen::Transform <A, B, C, D> &data)
  {
  return str >> data.matrix();
  }

template <typename A, int B, int C, int D, typename E, int F, int G, int H>
    bool operator ==(const Eigen::Transform <A, B, C, D> &a, const Eigen::Transform <E, F, G, H> &b)
  {
  return a.matrix() == b.matrix();
  }


template <typename A, int B, int C, int D, typename E, int F, int G, int H>
    bool operator !=(const Eigen::Transform <A, B, C, D> &a, const Eigen::Transform <E, F, G, H> &b)
  {
  return a.matrix() != b.matrix();
  }

typedef XList <XTransform> XTransformList;

#endif // XTRANSFORM_H
