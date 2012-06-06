#include "XTransform.h"

namespace XTransformUtilities
{
XTransform lookAt(const XVector3D &eye, const XVector3D &aim, const XVector3D &up)
  {
  XVector3D forward = (aim - eye).normalized();
  XVector3D side = forward.cross(up).normalized();
  XVector3D upVector = side.cross(forward);

  XTransform m;

  m.matrix().row(0).head<3>() = side;
  m.matrix().row(1).head<3>() = upVector;
  m.matrix().row(2).head<3>() = -forward;

  m.translate(-eye);

  return m;
  }

XComplexTransform perspective(xReal angle, xReal aspect, xReal nearPlane, xReal farPlane)
  {
  // Bail out if the projection volume is zero-sized.
  if (nearPlane == farPlane || aspect == 0.0f)
    {
    return XComplexTransform();
    }

  // Construct the projection.
  XComplexTransform m;
  m.matrix() = XMatrix4x4::Zero();

  qreal radians = (angle / 2.0f) * M_PI / 180.0f;
  qreal sine = sin(radians);
  if (sine == 0.0f)
    {
    return XComplexTransform();
    }

  qreal cotan = cos(radians) / sine;
  qreal clip = farPlane - nearPlane;

  m(0,0) = cotan / aspect;
  m(1,1) = cotan;
  m(2,2) = -(nearPlane + farPlane) / clip;
  m(2,3) = -(2.0f * nearPlane * farPlane) / clip;
  m(3,2) = -1.0f;

  return m;
  }
}
