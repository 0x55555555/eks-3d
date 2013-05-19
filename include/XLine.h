#ifndef XLINE_H
#define XLINE_H

#include "X3DGlobal.h"
#include "XMathVector"
#include "XProperty"
#include "XTransform.h"

namespace Eks
{

template <xsize Num> class LineBase
  {
public:
  typedef Eigen::Matrix<Real, Num, 1> VecType;
  typedef Eigen::Transform<Real, Num, Eigen::Affine> Transform;

XProperties:
  XByRefProperty(VecType, position, setPosition);
  XByRefProperty(VecType, direction, setDirection);

public:
  enum ConstructionMode
    {
    TwoPoints,
    PointAndDirection
    };

  LineBase(const VecType &p1, const VecType &p2, ConstructionMode mode=TwoPoints) : _position(p1)
    {
    if(mode == TwoPoints)
      {
      _direction = (p2-p1).normalized();
      }
    else
      {
      _direction = p2;
      }
    }

  VecType sample( float t ) const
    {
    return position() + ( t * direction() );
    }

  void transform(const Transform &tx)
    {
    _position = tx * _position;
    _direction = tx.linear() * _direction;
    }

  float closestPointTo(const VecType &l) const
    {
    VecType lineToPt = l - position();

    return direction().dot(lineToPt) / direction().squaredNorm();
    }

  VecType pointAtDistance(float distAlongLine) const
    {
    return position() + direction().normalized() * distAlongLine;
    }
  };

typedef LineBase<2> Line2D;
class EKS3D_EXPORT Line : public LineBase<3>
  {
public:
  Line(const VecType &p1, const VecType &p2, ConstructionMode mode=TwoPoints)
      : LineBase<3>(p1, p2, mode)
    {
    }

  float closestPointOn(const Line &l) const;
  };

template <xsize Num>
inline Eks::LineBase<Num> operator*( const typename LineBase<Num>::Transform &tx, const LineBase<Num> &line )
  {
  auto l(line);
  l.transform(tx);
  return l;
  }

}


#endif // XLINE_H
