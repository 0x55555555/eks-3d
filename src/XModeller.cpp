#include "XModeller.h"
#include "XGeometry.h"
#include "Math/XMathCurve.h"
#include "XBoundingBox.h"
#include "XShader.h"
#include "XGeometry.h"
#include "Math/XFrame.h"

namespace Eks
{

Modeller::Modeller(AllocatorBase *a, xsize initialSize )
  : _allocator(a),
    _triIndices(a),
    _linIndices(a),
    _vertex(a),
    _texture(a),
    _normals(a),
    _colours(a),
    _states(a),
    _transform(Transform::Identity())
  {
  _vertex.reserve(initialSize);
  _texture.reserve(initialSize);
  _normals.reserve(initialSize);
  _colours.reserve(initialSize);

  _areLineIndicesSequential = _areTriangleIndicesSequential = true;

  save( );
  }

Modeller::~Modeller( )
  {
  }

class Utils
  {
public:
  template <typename T>
      static void bakeArray(Vector<xuint8> &data, xsize offset, xsize stride, const Vector<T> &dataIn)
    {
    xsize count = dataIn.size();

    xAssert((offset + (stride * (count-1))) <= data.size());
    xuint8 *dataOut = data.data();
    for(xsize i = 0; i < count; ++i)
      {
      xuint8 *d = dataOut + offset + (stride * i);
      memcpy(d, dataIn[i].data(), sizeof(T));
      }
    }
  };

void Modeller::bakeVertices(
    Renderer *r,
    ShaderVertexLayoutDescription::Semantic *semanticOrder,
    xsize semanticCount,
    Geometry *geo)
  {
  Vector<xuint8> data(_allocator);

  xsize semanticSizes[] =
    {
    3,
    4,
    2,
    3,
    };
  xCompileTimeAssert(X_ARRAY_COUNT(semanticSizes) == ShaderVertexLayoutDescription::SemanticCount);

  xsize vertSize = 0;
  for(xsize i = 0; i < semanticCount; ++i)
    {
    vertSize += semanticSizes[semanticOrder[i]];
    }
  vertSize *= sizeof(float);

  data.resize(_vertex.size() * vertSize);

  xsize offset = 0;
  for(xsize i = 0; i < semanticCount; ++i)
    {
    ShaderVertexLayoutDescription::Semantic semantic = semanticOrder[i];
    if(semantic == ShaderVertexLayoutDescription::Position)
      {
      Utils::bakeArray(data, offset, vertSize, _vertex);
      offset += sizeof(float) * semanticSizes[semantic];
      }
    else if(semantic == ShaderVertexLayoutDescription::Normal)
      {
      Utils::bakeArray(data, offset, vertSize, _normals);
      offset += sizeof(float) * semanticSizes[semantic];
      }
    else if(semantic == ShaderVertexLayoutDescription::Colour)
      {
      Utils::bakeArray(data, offset, vertSize, _colours);
      offset += sizeof(float) * semanticSizes[semantic];
      }
    else if(semantic == ShaderVertexLayoutDescription::TextureCoordinate)
      {
      Utils::bakeArray(data, offset, vertSize, _texture);
      offset += sizeof(float) * semanticSizes[semantic];
      }
    }

  xsize elementCount = data.size() / vertSize;
  xAssert((data.size() % vertSize) == 0);

  Geometry::delayedCreate(*geo, r, data.data(), vertSize, elementCount);
  }

void Modeller::bakeTriangles(Renderer *r,
    ShaderVertexLayoutDescription::Semantic *semanticOrder,
    xsize semanticCount,
    IndexGeometry *index,
    Geometry *geo)
  {
  if(geo)
    {
    bakeVertices(r, semanticOrder, semanticCount, geo);
    }

  if(index)
    {
    xAssert(_triIndices.size() < X_UINT16_SENTINEL);

    IndexGeometry::delayedCreate(
      *index,
      r,
      IndexGeometry::Unsigned16,
      _triIndices.data(),
      _triIndices.size());
    }
  }

void Modeller::bakeLines(Renderer *r,
    ShaderVertexLayoutDescription::Semantic *semanticOrder,
    xsize semanticCount,
    IndexGeometry *index,
    Geometry *geo)
  {
  if(geo)
    {
    bakeVertices(r, semanticOrder, semanticCount, geo);
    }

  if(index)
    {
    xAssert(_linIndices.size() < X_UINT16_SENTINEL);

    IndexGeometry::delayedCreate(
      *index,
      r,
      IndexGeometry::Unsigned16,
      _linIndices.data(),
      _linIndices.size());
    }
  }


void Modeller::begin( Type type )
  {
  _quadCount = 0;
  _states.back().type = type;
  }

void Modeller::end( )
  {
  _states.back().type = None;
  }

void Modeller::vertex( const Vector3D &vec )
  {
  if( _normals.size() || !_states.back().normal.isZero() )
    {
    while( _normals.size() < _vertex.size() )
      {
      _normals << Vector3D::Zero();
      }
    _normals << transformNormal( _states.back().normal );
    }

  if( _texture.size() || !_states.back().texture.isZero() )
    {
    while( _texture.size() < _vertex.size() )
      {
      _texture << Eks::Vector2D::Zero();
      }
    _texture << _states.back().texture;
    }

  if( _colours.size() || !_states.back().colour.isZero() )
    {
    while( _colours.size() < _vertex.size() )
      {
      _colours << Eks::Vector4D::Zero();
      }
    _colours << _states.back().colour;
    }

  _vertex << transformPoint( vec );

  if( _states.back().type == Lines )
    {
    _areLineIndicesSequential |= _linIndices.size() == _vertex.size();
    _linIndices << (xuint16)(_vertex.size() - 1);
    }
  else if( _states.back().type == Triangles )
    {
    _areTriangleIndicesSequential |= _triIndices.size() == _vertex.size();
    _triIndices << (xuint16)(_vertex.size() - 1);

    if( _states.back().normalsAutomatic && ( _triIndices.size() % 3 ) == 0 )
      {
      _areTriangleIndicesSequential = false;
      xsize i1( _vertex.size() - 3 );
      xsize i2( _vertex.size() - 2 );
      xsize i3( _vertex.size() - 1 );
      while( _normals.size() < _vertex.size() )
        {
        _normals << Vector3D::Zero();
        }
      Vector3D vec1(_vertex[i2] - _vertex[i1]);
      Vector3D vec2(_vertex[i3] - _vertex[i1]);

      _normals[i1] = _normals[i2] = _normals[i3] = vec1.cross(vec2).normalized();
      }
    }
  else if( _states.back().type == Quads )
    {
    _quadCount++;
    _areTriangleIndicesSequential = false;
    _triIndices << (xuint16)(_vertex.size() - 1);

    if( _quadCount == 4 )
      {
      if( _states.back().normalsAutomatic )
        {
        xsize i1( _vertex.size() - 4 );
        xsize i2( _vertex.size() - 3 );
        xsize i3( _vertex.size() - 2 );
        xsize i4( _vertex.size() - 1 );
        while( _normals.size() < _vertex.size() )
          {
          _normals << Vector3D::Zero();
          }
        Vector3D vec1( _vertex[i2] - _vertex[i1]);
        Vector3D vec2( _vertex[i3] - _vertex[i1]);

        _normals[i1] = _normals[i2] = _normals[i3] = _normals[i4] = vec1.cross(vec2).normalized();
        }

      xsize idxA = _triIndices.size()-4;
      xsize idxB = _triIndices.size()-2;
      _triIndices << _triIndices[idxA];
      _triIndices << _triIndices[idxB];
      _quadCount = 0;
      }
    }
  }

void Modeller::normal( const Vector3D &norm )
  {
  _states.back().normal = norm;
  }

void Modeller::texture( const Eks::Vector2D &tex )
  {
  _states.back().texture = tex;
  }

void Modeller::colour( const Eks::Vector4D &col )
  {
  _states.back().colour = col;
  }

void Modeller::setNormalsAutomatic( bool nAuto )
  {
  _states.back().normalsAutomatic = nAuto;
  if( nAuto )
    {
    _states.back().normal = Vector3D::Zero();
    }
  }

bool Modeller::normalsAutomatic( ) const
  {
  return _states.back().normalsAutomatic;
  }

void Modeller::drawWireCube( const BoundingBox &cube )
  {
  _areLineIndicesSequential = false;

  Vector3D size = cube.size();
  Vector3D min = cube.minimum();

  xuint16 sI = (xuint16)_vertex.size();

  _vertex << min
          << min + Vector3D(size.x(), 0.0f, 0.0f)
          << min + Vector3D(size.x(), size.y(), 0.0f)
          << min + Vector3D(0.0f, size.y(), 0.0f)
          << min + Vector3D(0.0f, 0.0f, size.z())
          << min + Vector3D(size.x(), 0.0f, size.z())
          << min + size
          << min + Vector3D(0.0f, size.y(), size.z());

  Vector3D n(Vector3D::Zero());
  Vector2D t(Vector2D::Zero());
  _normals << n << n << n << n << n << n << n << n;
  _texture << t << t << t << t << t << t << t << t;
  _linIndices << sI << sI+1
              << sI+1 << sI+2
              << sI+2 << sI+3
              << sI+3 << sI

              << sI+4 << sI+5
              << sI+5 << sI+6
              << sI+6 << sI+7
              << sI+7 << sI+4

              << sI+4 << sI
              << sI+5 << sI+1
              << sI+6 << sI+2
              << sI+7 << sI+3;
  }

void Modeller::drawWireCircle(const Vector3D &pos, const Vector3D &normal, float radius, xsize pts)
  {
  _areLineIndicesSequential = false;

  Vector3D up = Vector3D(0,1,0);
  if(normal.dot(up) > 0.9f)
    {
    up = Vector3D(1, 0, 0);
    }
  Vector3D x = up.cross(normal);
  Vector3D y = normal.cross(x);

  xuint16 initialIndex = (xuint16)_vertex.size();
  for(xuint16 i = 0; i < (xuint16)pts; ++i)
    {
    float angle = i * (X_PI * 2.0f / (float)pts);
    xuint16 otherIndex = (xuint16)(i+1) % pts;

    _normals << Vector3D::Zero();
    _texture << Vector2D::Zero();
    _vertex << pos + (radius * (x * sinf(angle) + y * cosf(angle)));
    _linIndices << initialIndex + i << initialIndex + otherIndex;
    }
  }

void Modeller::drawCone(
    const Vector3D &point,
    const Vector3D &direction,
    float length,
    float radius,
    xuint32 divs,
    bool capped)
  {
  _areTriangleIndicesSequential = false;
  Vector3D dirNorm = direction.normalized();

  _vertex.reserve(1 + divs);
  _normals.reserve(1 + divs);
  _texture.reserve(1 + divs);
  _triIndices.reserve(3 * divs);

  xuint16 eIndex = (xuint16)(_vertex.size());
  _vertex << transformPoint(point + dirNorm * length);
  _normals << transformNormal(dirNorm);

  Eks::Vector2D t = Eks::Vector2D::Zero();

  Eks::Frame f(dirNorm);
  for(xuint32 i=0; i<divs; ++i)
    {
    float percent = (float)i/(float)divs * 2.0f * (float)M_PI;
    float c = cos(percent);
    float s = sin(percent);

    Vector3D ptDir = (f.up() * s) + (f.across() * c);

    _vertex << transformPoint(point + (ptDir * radius));
    _normals << transformNormal(ptDir);
    _texture << t;

    if(i == divs-1)
      {
      _triIndices << eIndex + i + 1 << eIndex << eIndex + 1;
      }
    else
      {
      _triIndices << eIndex + i + 1 << eIndex << eIndex + i + 2;
      }
    }

  if(capped)
    {
    for(xuint32 i=0; i<=(divs-2); ++i)
      {
      _triIndices << eIndex + 1 << eIndex + i + 1 << eIndex + i + 2;
      }
    }
  }

void Modeller::drawSphere(float r, int lats, int longs)
  {
  int i, j;
  for(i = 0; i <= lats; i++)
    {
    float lat0 = M_PI * (-0.5 + (float)(i - 1) / lats); 
    float z0  = sinf(lat0) * r;
    float zr0 = cosf(lat0) * r;

    float lat1 = M_PI * (-0.5 + (float)i / lats);
    float z1 = sinf(lat1) * r;
    float zr1 = cosf(lat1) * r;

    float vA = (float)i / (float)(lats+1);
    float vB = (float)(i+1) / (float)(lats+1);

    begin(Quads);
    for(j = 1; j <= longs; j++)
      {
      float lng = 2 * M_PI * (float) (j - 1) / longs;
      float x = cosf(lng);
      float y = sinf(lng);

      float lngOld = 2 * M_PI * (float) (j - 2) / longs;
      float xOld = cosf(lngOld);
      float yOld = sinf(lngOld);

      float uA = (float)(j-1)/(float)(longs);
      float uB = (float)(j)/(float)(longs);

      texture(uA, vB);
      normal(Eks::Vector3D(xOld * zr1, yOld * zr1, z1).normalized());
      vertex(xOld * zr1, yOld * zr1, z1);

      texture(uA, vA);
      normal(Eks::Vector3D(xOld * zr0, yOld * zr0, z0).normalized());
      vertex(xOld * zr0, yOld * zr0, z0);

      texture(uB, vA);
      normal(Eks::Vector3D(x * zr0, y * zr0, z0).normalized());
      vertex(x * zr0, y * zr0, z0);

      texture(uB, vB);
      normal(Eks::Vector3D(x * zr1, y * zr1, z1).normalized());
      vertex(x * zr1, y * zr1, z1);
      }
    end();
    }
  }

void Modeller::drawCube(
    const Vector3D &hor,
    const Vector3D &ver,
    const Vector3D &dep,
    float pX,
    float pY)
  {
  _areTriangleIndicesSequential = false;

  Vector3D h = hor * 0.5f;
  Vector3D v = ver * 0.5f;
  Vector3D d = dep * 0.5f;

  Vector3D p1( transformPoint( -h-v-d ) ),
      p2( transformPoint( h-v-d ) ),
      p3( transformPoint( h+v-d ) ),
      p4( transformPoint( -h+v-d ) ),
      p5( transformPoint( -h-v+d ) ),
      p6( transformPoint( h-v+d ) ),
      p7( transformPoint( h+v+d ) ),
      p8( transformPoint( -h+v+d ) );

  Vector3D n1( transformNormal( Vector3D(0,1,0) ) ),
      n2( transformNormal( Vector3D(0,-1,0) ) ),
      n3( transformNormal( Vector3D(1,0,0) ) ),
      n4( transformNormal( Vector3D(-1,0,0) ) ),
      n5( transformNormal( Vector3D(0,0,-1) ) ),
      n6( transformNormal( Vector3D(0,0,1) ) );

  // Top Face BL
  {
  xuint16 begin = (xuint16)_vertex.size();
  _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

  _normals << n1 << n1 << n1 << n1;
  _texture << Eks::Vector2D(1.0/3.0,pY) << Eks::Vector2D(1.0/3.0,0.5-pY) << Eks::Vector2D(pX,pY) << Eks::Vector2D(pX,0.5-pY);
  _vertex << p3 << p4 << p7 << p8;
  }

  // Back Face BM
  {
  xuint16 begin = (xuint16) _vertex.size();
  _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

  _normals << n5 << n5 << n5 << n5;
  _texture << Eks::Vector2D(2.0/3.0,pY) << Eks::Vector2D(2.0/3.0,0.5-pY) << Eks::Vector2D(1.0/3.0,pY) << Eks::Vector2D(1.0/3.0,0.5-pY);
  _vertex << p2 << p1 << p3 << p4;
  }

  // Bottom Face BR
  {
  xuint16 begin = (xuint16)_vertex.size();
  _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

  _normals << n2 << n2 << n2 << n2;
  _texture << Eks::Vector2D(2.0/3.0,0.5-pY) << Eks::Vector2D(2.0/3.0,pY) << Eks::Vector2D(1-pX,0.5-pY) << Eks::Vector2D(1-pX,pY);
  _vertex << p1 << p2 << p5 << p6;
  }

  // Left Face TL
  {
  xuint16 begin = (xuint16)_vertex.size();
  _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

  _normals << n3 << n3 << n3 << n3;
  _texture << Eks::Vector2D(1-pX,0.5+pY) << Eks::Vector2D(1-pX,1-pY) << Eks::Vector2D(2.0/3.0,0.5+pY) << Eks::Vector2D(2.0/3.0,1-pY);
  _vertex << p2 << p3 << p6 << p7;
  }

  // Front Face TM
  {
  xuint16 begin = (xuint16)_vertex.size();
  _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

  _normals << n6 << n6 << n6 << n6;
  _texture << Eks::Vector2D(1.0/3.0,0.5+pY) << Eks::Vector2D(2.0/3.0,0.5+pY) << Eks::Vector2D(1.0/3.0,1-pY) << Eks::Vector2D(2.0/3.0,1-pY);
  _vertex << p5 << p6 << p8 << p7;
  }

  // Right Face TR
  {
  xuint16 begin = (xuint16)_vertex.size();
  _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

  _normals << n4 << n4 << n4 << n4;
  _texture << Eks::Vector2D(pX,1-pY) << Eks::Vector2D(pX,0.5+pY) << Eks::Vector2D(1.0/3.0,1-pY) << Eks::Vector2D(1.0/3.0,0.5+pY);
  _vertex << p4 << p1 << p8 << p5;
  }
  }

void Modeller::drawQuad(const Vector3D &hor, const Vector3D &ver)
  {
  _areTriangleIndicesSequential = false;
  Vector3D h = hor / 2.0;
  Vector3D v = ver / 2.0;

  xuint16 begin = (xuint16)_vertex.size();
  _triIndices << begin << begin + 1 << begin + 2 << begin << begin + 2 << begin + 3;
  _vertex << transformPoint( -h - v ) << transformPoint( h - v ) << transformPoint( h + v ) << transformPoint( -h + v );
  _texture << Eks::Vector2D(0,0) << Eks::Vector2D(1,0) << Eks::Vector2D(1,1) << Eks::Vector2D(0,1);

  Vector3D norm( transformNormal( h.cross(v).normalized() ) );
  _normals << norm << norm << norm << norm;
  }

void Modeller::drawLocator(const Vector3D &size, const Vector3D &center)
  {
  xuint16 begin = (xuint16)_vertex.size();
  _linIndices << begin << begin + 1 << begin + 2 << begin + 3 << begin + 4 << begin + 5;

  _vertex << transformPoint( center + Vector3D( -size.x(), 0, 0 ) )
          << transformPoint( center + Vector3D( size.x(), 0, 0 ) )
          << transformPoint( center + Vector3D( 0, -size.y(), 0 ) )
          << transformPoint( center + Vector3D( 0, size.y(), 0 ) )
          << transformPoint( center + Vector3D( 0, 0, -size.z() ) )
          << transformPoint( center + Vector3D( 0, 0, size.z() ) );

  _texture << Eks::Vector2D() << Eks::Vector2D() << Eks::Vector2D() << Eks::Vector2D() << Eks::Vector2D() << Eks::Vector2D();
  _normals << Vector3D() << Vector3D() << Vector3D() << Vector3D() << Vector3D() << Vector3D();
  }

void Modeller::setTransform( const Transform &t )
  {
  _transform = t;
  }

Transform Modeller::transform( ) const
  {
  return _transform;
  }

void Modeller::save()
  {
  _states << State();
  }

void Modeller::restore()
  {
  if( _states.size() > 1 )
    {
    _states.popBack();
    }
  }

Vector3D Modeller::transformPoint( const Vector3D &in )
  {
  return _transform * in;
  }

void Modeller::transformPoints(Vector <Vector3D> &list)
  {
  if( _transform.isApprox(Transform::Identity()) )
    {
    return;
    }

  for(xsize i = 0, s = list.size(); i < s; ++i)
    {
    Vector3D& v = list[i];
    Vector3D tr = _transform * v;
    v = tr;
    }
  }

Vector3D Modeller::transformNormal( Vector3D in )
  {
  return _transform.linear() * in;
  }

void Modeller::transformNormals( Vector <Vector3D> &list, bool reNormalize )
  {
  if( _transform.isApprox(Transform::Identity()) )
    {
    return;
    }

  if(reNormalize)
    {
    for(xsize i = 0, s = list.size(); i < s; ++i)
      {
      Vector3D& v = list[i];
      Vector3D tr = (_transform.linear() * v).normalized();
      v = tr;
      }
    }
  else
    {
    for(xsize i = 0, s = list.size(); i < s; ++i)
      {
      Vector3D& v = list[i];
      Vector3D tr = _transform.linear() * v;
      v = tr;
      }
    }
  }

void Modeller::drawCurve(const AbstractCurve <Vector3D> &curve, xsize segments )
  {
  Real start( curve.minimumT() );
  Real inc( ( curve.maximumT() - curve.minimumT() ) / (segments-1) );

  xuint16 begin = (xuint16)_vertex.size();

  _vertex << transformPoint( curve.sample( start ) );
  _texture << Eks::Vector2D();
  _normals << Vector3D();

  for( xuint16 x=1; x<(xuint16)segments; x++ )
    {
    _linIndices << begin + (x-1) << begin + x;

    _vertex << transformPoint( curve.sample( start + ( x * inc ) ) );

    _texture << Eks::Vector2D();
    _normals << Vector3D();
    }
  }

}
