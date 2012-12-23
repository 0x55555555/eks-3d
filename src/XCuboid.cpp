#include "XCuboid.h"
#include "XLine.h"
#include "XPlane.h"

namespace Eks
{

Cuboid::Cuboid() : _isValid( false )
  {
  }

Cuboid::Cuboid( Vector3D minimum, Vector3D maximum ) : _minimum( minimum ), _maximum( maximum ), _isValid( true )
  {
  }

bool Cuboid::operator==(const Cuboid& oth) const
  {
  return _isValid == oth._isValid ||
      (_minimum == oth._minimum &&
       _maximum == oth._maximum);
  }

bool Cuboid::operator!=(const Cuboid& oth) const
  {
  return !(*this == oth);
  }

Vector3D Cuboid::centre() const
  {
  return (minimum() + maximum()) * 0.5;
  }

void Cuboid::unite( const Vector3D &in )
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

void Cuboid::unite( const Cuboid &in )
  {
  if(in.isValid())
    {
    unite( in.minimum() );
    unite( in.maximum() );
    }
  }

Cuboid Cuboid::united( const Cuboid &in ) const
  {
  if(in.isValid())
    {
    return united( in.minimum() ).united( in.maximum() );
    }
  return *this;
  }

Cuboid Cuboid::united( const Vector3D &in ) const
  {
  Cuboid ret( *this );
  ret.unite(in);
  return ret;
  }

Cuboid &Cuboid::operator|=( const Cuboid &in )
  {
  *this = *this | in;
  return *this;
  }

Cuboid Cuboid::operator|( const Cuboid &in ) const
  {
  return in.united( *this );
  }

void Cuboid::setMinimum( const Vector3D &in )
  {
  _isValid = true;
  _minimum = in;
  }

void Cuboid::setMaximum( const Vector3D &in )
  {
  _isValid = true;
  _maximum = in;
  }

Vector3D Cuboid::size() const
  {
  return _maximum - _minimum;
  }

Real Cuboid::maximumDistanceSquared() const
  {
  return Vector3D( xMax(_maximum.x(), -_minimum.x()),
                   xMax(_maximum.y(), -_minimum.y()),
                   xMax(_maximum.z(), -_minimum.z())).squaredNorm();
  }

bool Cuboid::intersects( const Cuboid &in ) const
  {
  if( this == &in )
    {
    return TRUE;
    }
  return in.maximum().x() < _minimum.x() && in.minimum().x() > _maximum.x() &&
      in.maximum().y() < _minimum.y() && in.minimum().y() > _maximum.y() &&
      in.maximum().z() < _minimum.z() && in.minimum().z() > _maximum.z();
  }

bool Cuboid::intersects( const Line &in, float &dist ) const
  {
  Vector3D t = in.sample(Plane(maximum(), Vector3D(0, 1, 0)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    dist = (in.position() - t).norm();
    return true;
    }

  t = in.sample(Plane(minimum(), Vector3D(0, -1, 0)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    dist = (in.position() - t).norm();
    return true;
    }

  t = in.sample(Plane(maximum(), Vector3D(1, 0, 0)).intersection(in));
  if( t.y() <= maximum().y() && t.y() >= minimum().y() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    dist = (in.position() - t).norm();
    return true;
    }

  t = in.sample(Plane(minimum(), Vector3D(-1, 0, 0)).intersection(in));
  if( t.y() <= maximum().y() && t.y() >= minimum().y() &&
      t.z() <= maximum().z() && t.z() >= minimum().z() )
    {
    dist = (in.position() - t).norm();
    return true;
    }

  t = in.sample(Plane(maximum(), Vector3D(0, 0, 1)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.y() <= maximum().y() && t.y() >= minimum().y() )
    {
    dist = (in.position() - t).norm();
    return true;
    }

  t = in.sample(Plane(minimum(), Vector3D(0, 0, -1)).intersection(in));
  if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
      t.y() <= maximum().y() && t.y() >= minimum().y() )
    {
    dist = (in.position() - t).norm();
    return true;
    }

  return false;
  }

bool Cuboid::isInside( const Vector3D &in ) const
  {
  return in.x() < _minimum.x() && in.x() > _maximum.x() &&
      in.y() < _minimum.y() && in.y() > _maximum.y() &&
      in.z() < _minimum.z() && in.z() > _maximum.z();
  }

void Cuboid::expand(float amount)
  {
  Vector3D vec(amount, amount, amount);

  _minimum -= vec;
  _maximum += vec;
  }


QDataStream &operator<<(QDataStream &stream, const Cuboid &cuboid)
  {
  return stream << cuboid._isValid
                << cuboid._maximum
                << cuboid._minimum;
  }

QDataStream &operator>>(QDataStream &stream, Cuboid &cuboid)
  {
  return stream >> cuboid._isValid
                >> cuboid._maximum
                >> cuboid._minimum;
  }

QTextStream &operator<<(QTextStream& str, const Cuboid& cub)
  {
  return str << (xuint32)cub.isValid() << cub.minimum() << cub.maximum();
  }

QTextStream &operator>>(QTextStream& str, Cuboid& cub)
  {
  cub = Cuboid();
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

}
