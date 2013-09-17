#include "XTransform.h"

namespace Eks
{

namespace TransformUtilities
{
Transform lookAt(const Vector3D &eye, const Vector3D &aim, const Vector3D &up)
  {
  Vector3D forward = (aim - eye).normalized();
  Vector3D side = forward.cross(up).normalized();
  Vector3D upVector = side.cross(forward);

  Transform m = Transform::Identity();

  m.matrix().row(0).head<3>() = side;
  m.matrix().row(1).head<3>() = upVector;
  m.matrix().row(2).head<3>() = -forward;

  m.translate(-eye);

  return m;
  }

ComplexTransform perspective(Real angle, Real aspect, Real nearPlane, Real farPlane)
  {
  // Bail out if the projection volume is zero-sized.
  if (nearPlane == farPlane || aspect == 0.0f)
    {
    return ComplexTransform::Identity();
    }

  // Construct the projection.
  ComplexTransform m;
  m.matrix() = Eks::Matrix4x4::Zero();

  Real sine = Eks::sin(angle);
  if (sine == 0.0f)
    {
    return ComplexTransform::Identity();
    }

  Real cotan = Eks::cos(angle) / sine;
  Real clip = farPlane - nearPlane;

  m(0,0) = cotan / aspect;
  m(1,1) = cotan;
  m(2,2) = -(nearPlane + farPlane) / clip;
  m(2,3) = -(2.0f * nearPlane * farPlane) / clip;
  m(3,2) = -1.0f;

  return m;
  }
}

}
