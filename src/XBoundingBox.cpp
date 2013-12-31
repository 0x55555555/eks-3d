#include "XBoundingBox.h"
#include "XLine.h"
#include "XPlane.h"


namespace Eks
{

BoundingBox::BoundingBox() : _isValid( false )
  {
  }

BoundingBox::BoundingBox( Vector3D minimum, Vector3D maximum ) : _minimum( minimum ), _maximum( maximum ), _isValid( true )
  {
  }

void BoundingBox::clear()
  {
  _isValid = false;
  }

bool BoundingBox::operator==(const BoundingBox& oth) const
  {
  return _isValid == oth._isValid ||
      (_minimum == oth._minimum &&
       _maximum == oth._maximum);
  }

bool BoundingBox::operator!=(const BoundingBox& oth) const
  {
  return !(*this == oth);
  }

Vector3D BoundingBox::centre() const
  {
  return (minimum() + maximum()) * 0.5;
  }

void BoundingBox::unite( const Vector3D &in )
  {
  if( !isValid() )
    {
    _minimum = in;
    _maximum = in;
    _isValid = true;
    }
  else
    {
    if( in.x() < _minimum.x() )
      {
      _minimum(0) = in.x();
      }
    if( in.x() > _maximum.x() )
      {
      _maximum(0) = in.x();
      }
    if( in.y() < _minimum.y() )
      {
      _minimum(1) = in.y();
      }
    if( in.y() > _maximum.y() )
      {
      _maximum(1) = in.y();
      }
    if( in.z() < _minimum.z() )
      {
      _minimum(2) = in.z();
      }
    if( in.z() > _maximum.z() )
      {
      _maximum(2) = in.z();
      }
    }
  }

void BoundingBox::unite( const BoundingBox &in )
  {
  if(in.isValid())
    {
    unite( in.minimum() );
    unite( in.maximum() );
    }
  }

BoundingBox BoundingBox::united( const BoundingBox &in ) const
  {
  if(in.isValid())
    {
    return united( in.minimum() ).united( in.maximum() );
    }
  return *this;
  }

BoundingBox BoundingBox::united( const Vector3D &in ) const
  {
  BoundingBox ret( *this );
  ret.unite(in);
  return ret;
  }

BoundingBox &BoundingBox::operator|=( const BoundingBox &in )
  {
  *this = *this | in;
  return *this;
  }

BoundingBox BoundingBox::operator|( const BoundingBox &in ) const
  {
  return in.united( *this );
  }

void BoundingBox::setMinimum( const Vector3D &in )
  {
  _isValid = true;
  _minimum = in;
  }

void BoundingBox::setMaximum( const Vector3D &in )
  {
  _isValid = true;
  _maximum = in;
  }

Vector3D BoundingBox::size() const
  {
  return _maximum - _minimum;
  }

Real BoundingBox::maximumDistanceSquared() const
  {
  return Vector3D( xMax(_maximum.x(), -_minimum.x()),
                   xMax(_maximum.y(), -_minimum.y()),
                   xMax(_maximum.z(), -_minimum.z())).squaredNorm();
  }

void BoundingBox::buildExtremities(Eks::Vector3D (&pts)[8]) const
  {
  enum { X = 1, Y = 2, Z = 4 };

  for (xsize i = 0; i < X_ARRAY_COUNT(pts); ++i)
    {
    Eks::VectorUI3D sel(i & X, i & Y, i & Z);

    pts[i] = sel.select(_maximum, _minimum);
    }
  }

bool BoundingBox::intersects( const BoundingBox &in ) const
  {
  if( this == &in )
    {
    return true;
    }
  return in.maximum().x() < _minimum.x() && in.minimum().x() > _maximum.x() &&
      in.maximum().y() < _minimum.y() && in.minimum().y() > _maximum.y() &&
      in.maximum().z() < _minimum.z() && in.minimum().z() > _maximum.z();
  }

bool BoundingBox::intersects(const Line &in, float &outT) const
  {
  Vector3D t = in.sample(Plane(maximum(), Vector3D(0, 1, 0)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    outT = (t - in.position()).norm() / in.direction().norm();
    return true;
    }

  t = in.sample(Plane(minimum(), Vector3D(0, -1, 0)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    outT = (t - in.position()).norm() / in.direction().norm();
    return true;
    }

  t = in.sample(Plane(maximum(), Vector3D(1, 0, 0)).intersection(in));
  if( t.y() <= maximum().y() && t.y() >= minimum().y() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    outT = (t - in.position()).norm() / in.direction().norm();
    return true;
    }

  t = in.sample(Plane(minimum(), Vector3D(-1, 0, 0)).intersection(in));
  if( t.y() <= maximum().y() && t.y() >= minimum().y() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    outT = (t - in.position()).norm() / in.direction().norm();
    return true;
    }

  t = in.sample(Plane(maximum(), Vector3D(0, 0, 1)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.y() <= maximum().y() && t.y() >= minimum().y() )
    {
    outT = (t - in.position()).norm() / in.direction().norm();
    return true;
    }

  t = in.sample(Plane(minimum(), Vector3D(0, 0, -1)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.y() <= maximum().y() && t.y() >= minimum().y() )
    {
    outT = (t - in.position()).norm() / in.direction().norm();
    return true;
    }

  return false;
  }

bool BoundingBox::contains( const Vector3D &in ) const
  {
  return in.x() <= _minimum.x() && in.x() >= _maximum.x() &&
         in.y() <= _minimum.y() && in.y() >= _maximum.y() &&
         in.z() <= _minimum.z() && in.z() >= _maximum.z();
  }

bool BoundingBox::contains( const BoundingBox &smaller ) const
  {
  return smaller.minimum().x() >= _minimum.x() && smaller.maximum().x() <= _maximum.x() &&
         smaller.minimum().y() >= _minimum.y() && smaller.maximum().y() <= _maximum.y() &&
         smaller.minimum().z() >= _minimum.z() && smaller.maximum().z() <= _maximum.z();
  }

void BoundingBox::expand(float amount)
  {
  Vector3D vec(amount, amount, amount);

  _minimum -= vec;
  _maximum += vec;
  }

std::ostream &operator<<(std::ostream& str, const BoundingBox& cub)
  {
  return str << (xuint32)cub.isValid() << cub.minimum() << cub.maximum();
  }

std::istream &operator>>(std::istream& str, BoundingBox& cub)
  {
  using ::operator >>;

  cub = BoundingBox();
  xuint32 valid = false;
  Vector3D min, max;
  str >> valid >> min >> max;
  if(valid)
    {
    cub.setMinimum(min);
    cub.setMaximum(max);
    }
  return str;
  }

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
  : _frame(Eks::Vector3D(0.0f, 0.0f, 1.0f))
  {
  }

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Frame &f, const BoundingBox &bnds)
  : _frame(f),
    _bounds(bnds)
  {
  }

void AxisAlignedBoundingBox::expandAligned(const BoundingBox &bnds)
  {
  Eks::Vector3D pts[8];
  bnds.buildExtremities(pts);

  const Eks::Vector3D *ax = frame().axes();

  xForeach(const auto& pt, pts)
    {
    Eks::Vector3D dotted[3];
    for(xsize i = 0; i < X_ARRAY_COUNT(dotted); ++i)
      {
      dotted[i] = ax[i] * pt.dot(ax[i]);

      _bounds.unite(dotted[i]);
      }
    }
  }

void AxisAlignedBoundingBox::buildExtremities(Vector3D (&pts)[8]) const
  {
  // find the extremitied in local space.
  _bounds.buildExtremities(pts);

  Eks::Transform myTr = frame().transform();

  // rotate each point into world space.
  for(xsize i = 0; i < X_ARRAY_COUNT(pts); ++i)
    {
    Eks::Vector3D &pt = pts[i];

    pt = myTr * pt;
    }
  }

void AxisAlignedBoundingBox::maximumExtents(const Eks::Vector3D &vec, float &min, float &max) const
  {
  min = HUGE_VAL;
  max = -HUGE_VAL;

  // Find the extremitied in world space.
  Vector3D pts[8];
  buildExtremities(pts);

  // project each point onto vec and find min and max.
  for(xsize i = 0; i < X_ARRAY_COUNT(pts); ++i)
    {
    const Eks::Vector3D &pt = pts[i];

    float proj = vec.dot(pt);

    min = xMin(min, proj);
    max = xMax(max, proj);
    }
  }
}
