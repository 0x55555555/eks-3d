#ifndef XTRANSFORM_H
#define XTRANSFORM_H

#include "X3DGlobal.h"
#undef min
#undef max
#include "Eigen/Geometry"
#include "XMathVector"
#include "XMathMatrix"

namespace Eks
{

typedef Eigen::Affine3f Transform;
typedef Eigen::Projective3f ComplexTransform;

namespace TransformUtilities
{
Transform EKS3D_EXPORT lookAt(const Vector3D &eye, const Vector3D &aim, const Vector3D &up);
ComplexTransform EKS3D_EXPORT perspective(Real angle, Real aspect, Real nearPlane, Real farPlane);
}
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

#endif // XTRANSFORM_H
