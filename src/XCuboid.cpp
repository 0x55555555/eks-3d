#include "XCuboid.h"
#include "XLine.h"
#include "XPlane.h"

XCuboid::XCuboid() : _isValid( false )
  {
  }

XCuboid::XCuboid( XVector3D minimum, XVector3D maximum ) : _minimum( minimum ), _maximum( maximum ), _isValid( true )
  {
  }

XCuboid::XCuboid( XVector3D minimum, XSize size ) : _minimum( minimum ), _maximum( _minimum + size ), _isValid( true )
  {
  }

bool XCuboid::operator==(const XCuboid& oth) const
  {
  return _isValid == oth._isValid ||
      (_minimum == oth._minimum &&
      _maximum == oth._maximum);
  }

bool XCuboid::operator!=(const XCuboid& oth) const
  {
  return !(*this == oth);
  }

XVector3D XCuboid::centre() const
  {
  return (minimum() + maximum()) * 0.5;
  }

void XCuboid::unite( const XVector3D &in )
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

void XCuboid::unite( const XCuboid &in )
  {
  if(in.isValid())
    {
    unite( in.minimum() );
    unite( in.maximum() );
    }
  }

XCuboid XCuboid::united( const XCuboid &in ) const
  {
  if(in.isValid())
    {
    return united( in.minimum() ).united( in.maximum() );
    }
  return *this;
  }

XCuboid XCuboid::united( const XVector3D &in ) const
  {
  XCuboid ret( *this );
  ret.unite(in);
  return ret;
  }

XCuboid &XCuboid::operator|=( const XCuboid &in )
  {
  *this = *this | in;
  return *this;
  }

XCuboid XCuboid::operator|( const XCuboid &in ) const
  {
  return in.united( *this );
  }

void XCuboid::setMinimum( const XVector3D &in )
  {
  _isValid = true;
  _minimum = in;
  }

void XCuboid::setMaximum( const XVector3D &in )
  {
  _isValid = true;
  _maximum = in;
  }

XSize XCuboid::size() const
  {
  return XSize( _maximum - _minimum );
  }

xReal XCuboid::maximumDistanceSquared() const
  {
  return XVector3D( xMax(_maximum.x(), -_minimum.x()),
                    xMax(_maximum.y(), -_minimum.y()),
                    xMax(_maximum.z(), -_minimum.z())).squaredNorm();
  }

bool XCuboid::intersects( const XCuboid &in ) const
  {
  if( this == &in )
    {
    return TRUE;
    }
  return in.maximum().x() < _minimum.x() && in.minimum().x() > _maximum.x() &&
          in.maximum().y() < _minimum.y() && in.minimum().y() > _maximum.y() &&
          in.maximum().z() < _minimum.z() && in.minimum().z() > _maximum.z();
  }

bool XCuboid::intersects( const XLine &in, float &dist ) const
    {
    XVector3D t = in.sample(XPlane(maximum(), XVector3D(0, 1, 0)).intersection(in));
    if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
        t.z() <= maximum().z() && t.z() >= minimum().z() )
      {
      dist = (in.position() - t).norm();
      return true;
      }

    t = in.sample(XPlane(minimum(), XVector3D(0, -1, 0)).intersection(in));
    if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
        t.z() <= maximum().z() && t.z() >= minimum().z() )
      {
      dist = (in.position() - t).norm();
      return true;
      }

    t = in.sample(XPlane(maximum(), XVector3D(1, 0, 0)).intersection(in));
    if( t.y() <= maximum().y() && t.y() >= minimum().y() &&
        t.z() <= maximum().z() && t.z() >= minimum().z() )
      {
      dist = (in.position() - t).norm();
      return true;
      }

    t = in.sample(XPlane(minimum(), XVector3D(-1, 0, 0)).intersection(in));
    if( t.y() <= maximum().y() && t.y() >= minimum().y() &&
        t.z() <= maximum().z() && t.z() >= minimum().z() )
      {
      dist = (in.position() - t).norm();
      return true;
      }

    t = in.sample(XPlane(maximum(), XVector3D(0, 0, 1)).intersection(in));
    if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
        t.y() <= maximum().y() && t.y() >= minimum().y() )
      {
      dist = (in.position() - t).norm();
      return true;
      }

    t = in.sample(XPlane(minimum(), XVector3D(0, 0, -1)).intersection(in));
    if( t.x() <= maximum().x() && t.x() >= minimum().x() &&
        t.y() <= maximum().y() && t.y() >= minimum().y() )
      {
      dist = (in.position() - t).norm();
      return true;
      }

    return false;
    }

bool XCuboid::isInside( const XVector3D &in ) const
    {
    return in.x() < _minimum.x() && in.x() > _maximum.x() &&
            in.y() < _minimum.y() && in.y() > _maximum.y() &&
            in.z() < _minimum.z() && in.z() > _maximum.z();
    }

void XCuboid::expand(float amount)
  {
  XVector3D vec(amount, amount, amount);

  _minimum -= vec;
  _maximum += vec;
  }


QDataStream &operator<<(QDataStream &stream, const XCuboid &cuboid)
  {
  return stream << cuboid._isValid
                << cuboid._maximum
                << cuboid._minimum;
  }

QDataStream &operator>>(QDataStream &stream, XCuboid &cuboid)
  {
  return stream >> cuboid._isValid
                >> cuboid._maximum
                >> cuboid._minimum;
  }

QTextStream &operator<<(QTextStream& str, const XCuboid& cub)
  {
  return str << (xuint32)cub.isValid() << cub.minimum() << cub.maximum();
  }

QTextStream &operator>>(QTextStream& str, XCuboid& cub)
  {
  cub = XCuboid();
  xuint32 valid = false;
  XVector3D min, max;
  str >> valid >> min >> max;
  if(valid)
    {
    cub.setMinimum(min);
    cub.setMaximum(max);
    }
  return str;
  }
