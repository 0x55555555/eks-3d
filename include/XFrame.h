#ifndef XFRAME_H
#define XFRAME_H

#include "Utilities/XProperty.h"
#include "Math/XMathVector.h"
#include "XTransform.h"

namespace Eks
{

class Frame
  {
public:
  Frame(
      const Vector3D &facingIn,
      const Vector3D &upIn = Eks::Vector3D(0.0f, 1.0f, 0.0f),
      const Vector3D &alternativeUp = Eks::Vector3D(1.0f, 0.0f, 0.0f))
    {
    _axes[0] = facingIn.normalized();
    _axes[1] = upIn;
    if(up().dot(facing()) > 0.8f)
      {
      _axes[1] = alternativeUp;
      }
    _axes[2] = up().cross(_axes[0]).normalized();
    _axes[1] = _axes[0].cross(_axes[2]).normalized();
    }

  const Vector3D &facing() const { return _axes[0]; }
  const Vector3D &up() const { return _axes[1]; }
  const Vector3D &across() const { return _axes[2]; }

  Transform transform() const
    {
    Transform tr;
    for(xsize i = 0; i < X_ARRAY_COUNT(_axes); ++i)
      {
      tr.matrix().col(i).head<3>() = _axes[i];
      }

	tr.translation() = Eks::Vector3D::Zero();

    return tr;
    }

  void setFacing(const Vector3D &facing)
    {
    _axes[0] = facing;
    }

  void setUp(const Vector3D &up)
    {
    _axes[1] = up;
    }

  void setAcross(const Vector3D &across)
    {
    _axes[2] = across;
    }

  const Vector3D *axes() const { return _axes; }

  static Frame XYZ()
    {
    Frame f;
    f.setFacing(Eks::Vector3D(1.0f, 0.0f, 0.0f));
    f.setUp(Eks::Vector3D(0.0f, 1.0f, 0.0f));
    f.setAcross(Eks::Vector3D(0.0f, 0.0f, 1.0f));
    return f;
    }

private:
  Frame()
    {
    }

  Vector3D _axes[3];
  };


}

#endif // XFRAME_H
