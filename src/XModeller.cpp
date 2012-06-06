#include "XModeller.h"
#include "XGeometry.h"
#include "XCurve"
#include "XSize"
#include "XCuboid.h"

XModeller::XModeller( XGeometry *g, xsize initialSize ) : _geo( g ), _transform(XTransform::Identity())
    {
    _vertex.reserve(initialSize);
    _texture.reserve(initialSize);
    _normals.reserve(initialSize);
    _colours.reserve(initialSize);
    save( );
    }

XModeller::~XModeller( )
    {
    flush( );
    }

void XModeller::flush( )
    {
    _geo->setTriangles( _triIndices );
    _geo->setLines(  _linIndices );
    _geo->setPoints( _poiIndices );
    _geo->setAttribute( "vertex", _vertex );
    if( _texture.size() )
        {
        _geo->setAttribute( "texture", _texture );
        }
    if( _normals.size() )
        {
        _geo->setAttribute( "normal", _normals );
        }
    if( _colours.size() )
        {
        _geo->setAttribute( "colour", _colours );
        }
    }


void XModeller::begin( Type type )
    {
    _quadCount = 0;
    _states.back().type = type;
    }

void XModeller::end( )
    {
    _states.back().type = None;
    }

void XModeller::vertex( const XVector3D &vec )
    {
    if( _normals.size() || _states.back().normal.isZero() )
        {
        while( _normals.size() < _vertex.size() )
            {
            _normals << XVector3D();
            }
        _normals << transformNormal( _states.back().normal );
        }

    if( _texture.size() || _states.back().texture.isZero() )
        {
        while( _texture.size() < _vertex.size() )
            {
            _texture << XVector2D();
            }
        _texture << _states.back().texture;
        }

    if( _colours.size() || _states.back().colour.isZero() )
        {
        while( _colours.size() < _vertex.size() )
            {
            _colours << XVector4D();
            }
        _colours << _states.back().colour;
        }

    _vertex << transformPoint( vec );

    if( _states.back().type == Points )
        {
        _poiIndices << _vertex.size() - 1;
        }
    else if( _states.back().type == Lines )
        {
        _linIndices << _vertex.size() - 1;
        }
    else if( _states.back().type == Triangles )
        {
        _triIndices << _vertex.size() - 1;

        if( _states.back().normalsAutomatic && ( _triIndices.size() % 3 ) == 0 )
            {
            int i1( _vertex.size() - 3 );
            int i2( _vertex.size() - 2 );
            int i3( _vertex.size() - 1 );
            while( _normals.size() < _vertex.size() )
                {
                _normals << XVector3D();
                }
            XVector3D vec1(_vertex[i2] - _vertex[i1]);
            XVector3D vec2(_vertex[i3] - _vertex[i1]);

            _normals[i1] = _normals[i2] = _normals[i3] = vec1.cross(vec2).normalized();
            }
        }
    else if( _states.back().type == Quads )
        {
        _quadCount++;
        _triIndices << _vertex.size() - 1;

        if( _quadCount == 4 )
            {
            if( _states.back().normalsAutomatic )
                {
                int i1( _vertex.size() - 4 );
                int i2( _vertex.size() - 3 );
                int i3( _vertex.size() - 2 );
                int i4( _vertex.size() - 1 );
                while( _normals.size() < _vertex.size() )
                    {
                    _normals << XVector3D();
                    }
                XVector3D vec1( _vertex[i2] - _vertex[i1]);
                XVector3D vec2( _vertex[i3] - _vertex[i1]);

                _normals[i1] = _normals[i2] = _normals[i3] = _normals[i4] = vec1.cross(vec2).normalized();
                }

            _triIndices << _triIndices[_triIndices.size()-4] << _triIndices[_triIndices.size()-2];
            _quadCount = 0;
            }
        }
    }

void XModeller::normal( const XVector3D &norm )
    {
    _states.back().normal = norm;
    }

void XModeller::texture( const XVector2D &tex )
    {
    _states.back().texture = tex;
    }

void XModeller::colour( const XVector4D &col )
    {
    _states.back().colour = col;
    }

void XModeller::setNormalsAutomatic( bool nAuto )
    {
    _states.back().normalsAutomatic = nAuto;
    if( nAuto )
        {
        _states.back().normal = XVector3D();
        }
    }

bool XModeller::normalsAutomatic( ) const
    {
    return _states.back().normalsAutomatic;
    }

void XModeller::drawWireCube( const XCuboid &cube )
    {
    XSize size = cube.size();
    XVector3D min = cube.minimum();

    int sI = _vertex.size();

    _vertex << min
        << min + XVector3D(size.x(), 0.0f, 0.0f)
        << min + XVector3D(size.x(), size.y(), 0.0f)
        << min + XVector3D(0.0f, size.y(), 0.0f)
        << min + XVector3D(0.0f, 0.0f, size.z())
        << min + XVector3D(size.x(), 0.0f, size.z())
        << min + size
        << min + XVector3D(0.0f, size.y(), size.z());

    XVector3D n;
    XVector2D t;
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

void XModeller::drawCone(const XVector3D &point, const XVector3D &direction, float length, float radius, xuint32 divs)
  {
  XVector3D dirNorm = direction.normalized();

  _vertex.reserve(1 + divs);
  _normals.reserve(1 + divs);
  _texture.reserve(1 + divs);
  _triIndices.reserve(3 * divs);

  xuint32 eIndex = _vertex.size();
  _vertex << transformPoint(point + dirNorm * length);
  _normals << transformNormal(dirNorm);

  XVector2D t = XVector2D::Zero();

  XVector3D up = XVector3D(0.0f, 1.0f, 0.0f);
  if(up.dot(dirNorm) > 0.98f)
    {
    up = XVector3D(1.0f, 0.0f, 0.0f);
    }
  XVector3D across = dirNorm.cross(up).normalized();

  for(xuint32 i=0; i<divs; ++i)
    {
    float percent = (float)i/(float)divs * 2.0f * (float)M_PI;
    float c = cos(percent);
    float s = sin(percent);

    XVector3D ptDir = (up * s) + (across * c);

    _vertex << transformPoint(point + (ptDir * radius));
    _normals << transformNormal(ptDir);
    _texture << t;

    if(i == divs-1)
      {
      _triIndices << eIndex << eIndex + i + 1 << eIndex + 1;
      }
    else
      {
      _triIndices << eIndex << eIndex + i + 1 << eIndex + i + 2;
      }
    }
  }

void XModeller::drawCube( XVector3D h, XVector3D v, XVector3D d, float pX, float pY )
    {
    h *= 0.5; v *= 0.5; d *= 0.5;

    XVector3D p1( transformPoint( -h-v-d ) ),
              p2( transformPoint( h-v-d ) ),
              p3( transformPoint( h+v-d ) ),
              p4( transformPoint( -h+v-d ) ),
              p5( transformPoint( -h-v+d ) ),
              p6( transformPoint( h-v+d ) ),
              p7( transformPoint( h+v+d ) ),
              p8( transformPoint( -h+v+d ) );

    XVector3D n1( transformNormal( XVector3D(0,1,0) ) ),
              n2( transformNormal( XVector3D(0,-1,0) ) ),
              n3( transformNormal( XVector3D(1,0,0) ) ),
              n4( transformNormal( XVector3D(-1,0,0) ) ),
              n5( transformNormal( XVector3D(0,0,-1) ) ),
              n6( transformNormal( XVector3D(0,0,1) ) );

    // Top Face BL
        {
        unsigned int begin( _vertex.size() );
        _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

        _normals << n1 << n1 << n1 << n1;
        _texture << XVector2D(1.0/3.0,pY) << XVector2D(1.0/3.0,0.5-pY) << XVector2D(pX,pY) << XVector2D(pX,0.5-pY);
        _vertex << p3 << p4 << p7 << p8;
        }

    // Back Face BM
        {
        unsigned int begin( _vertex.size() );
        _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

        _normals << n5 << n5 << n5 << n5;
        _texture << XVector2D(2.0/3.0,pY) << XVector2D(2.0/3.0,0.5-pY) << XVector2D(1.0/3.0,pY) << XVector2D(1.0/3.0,0.5-pY);
        _vertex << p2 << p1 << p3 << p4;
        }

    // Bottom Face BR
        {
        unsigned int begin( _vertex.size() );
        _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

        _normals << n2 << n2 << n2 << n2;
        _texture << XVector2D(2.0/3.0,0.5-pY) << XVector2D(2.0/3.0,pY) << XVector2D(1-pX,0.5-pY) << XVector2D(1-pX,pY);
        _vertex << p1 << p2 << p5 << p6;
        }

    // Left Face TL
        {
        unsigned int begin( _vertex.size() );
        _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

        _normals << n3 << n3 << n3 << n3;
        _texture << XVector2D(1-pX,0.5+pY) << XVector2D(1-pX,1-pY) << XVector2D(2.0/3.0,0.5+pY) << XVector2D(2.0/3.0,1-pY);
        _vertex << p2 << p3 << p6 << p7;
        }

    // Front Face TM
        {
        unsigned int begin( _vertex.size() );
        _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

        _normals << n6 << n6 << n6 << n6;
        _texture << XVector2D(1.0/3.0,0.5+pY) << XVector2D(2.0/3.0,0.5+pY) << XVector2D(1.0/3.0,1-pY) << XVector2D(2.0/3.0,1-pY);
        _vertex << p5 << p6 << p8 << p7;
        }

    // Right Face TR
        {
        unsigned int begin( _vertex.size() );
        _triIndices << begin << begin + 1 << begin + 2 << begin + 2 << begin + 1 << begin + 3;

        _normals << n4 << n4 << n4 << n4;
        _texture << XVector2D(pX,1-pY) << XVector2D(pX,0.5+pY) << XVector2D(1.0/3.0,1-pY) << XVector2D(1.0/3.0,0.5+pY);
        _vertex << p4 << p1 << p8 << p5;
        }
    }

void XModeller::drawQuad( XVector3D h, XVector3D v )
    {
    h /= 2.0; v /= 2.0;

    unsigned int begin( _vertex.size() );
    _triIndices << begin << begin + 1 << begin + 2 << begin << begin + 2 << begin + 3;
    _vertex << transformPoint( -h - v ) << transformPoint( h - v ) << transformPoint( h + v ) << transformPoint( -h + v );
    _texture << XVector2D(0,0) << XVector2D(1,0) << XVector2D(1,1) << XVector2D(0,1);

    XVector3D norm( transformNormal( h.cross(v).normalized() ) );
    _normals << norm << norm << norm << norm;
    }

void XModeller::drawGeometry( const XGeometry &geo )
    {
    unsigned int begin( _vertex.size() );

    _vertex << transformPoints( geo.attributes3D()["vertex"] );
    _texture << geo.attributes2D()["texture"];
    _normals << transformNormals( geo.attributes3D()["normal"] );

    foreach( const unsigned int &i, geo.triangles() )
        {
        _triIndices << begin + i;
        }
    foreach( const unsigned int &i, geo.lines() )
        {
        _linIndices << begin + i;
        }
    foreach( const unsigned int &i, geo.points() )
        {
        _poiIndices << begin + i;
        }
    }

void XModeller::drawGeometry( XList <XVector3D> positions, const XGeometry &geo )
    {
    foreach( const XVector3D &pos, positions )
        {
        unsigned int begin( _vertex.size() );
        foreach( const XVector3D &curPos, geo.attributes3D()["vertex"] )
            {
            _vertex << transformPoint(curPos + pos);
            }
        _texture << geo.attributes2D()["texture"];
        _normals << transformNormals( geo.attributes3D()["normal"] );

        foreach( const unsigned int &i, geo.triangles() )
            {
            _triIndices << begin + i;
            }
        foreach( const unsigned int &i, geo.lines() )
            {
            _linIndices << begin + i;
            }
        foreach( const unsigned int &i, geo.points() )
            {
            _poiIndices << begin + i;
            }
        }
    }

void XModeller::drawLocator( XSize size, XVector3D center )
    {
    int begin( _vertex.size() );
    _linIndices << begin << begin + 1 << begin + 2 << begin + 3 << begin + 4 << begin + 5;

    _vertex << transformPoint( center + XVector3D( -size.x(), 0, 0 ) )
                      << transformPoint( center + XVector3D( size.x(), 0, 0 ) )
                      << transformPoint( center + XVector3D( 0, -size.y(), 0 ) )
                      << transformPoint( center + XVector3D( 0, size.y(), 0 ) )
                      << transformPoint( center + XVector3D( 0, 0, -size.z() ) )
                      << transformPoint( center + XVector3D( 0, 0, size.z() ) );

    _texture << XVector2D() << XVector2D() << XVector2D() << XVector2D() << XVector2D() << XVector2D();
    _normals << XVector3D() << XVector3D() << XVector3D() << XVector3D() << XVector3D() << XVector3D();
    }

void XModeller::setTransform( const XTransform &t )
    {
    _transform = t;
    }

XTransform XModeller::transform( ) const
    {
    return _transform;
    }

void XModeller::save()
    {
    _states << State();
    }

void XModeller::restore()
    {
    if( _states.size() > 1 )
        {
        _states.pop_back();
        }
    }

XVector3D XModeller::transformPoint( const XVector3D &in )
    {
    return _transform * in;
    }

XVector <XVector3D> XModeller::transformPoints( const XVector <XVector3D> &list )
    {
    if( _transform.isApprox(XTransform::Identity()) )
        {
        return list;
        }

    XVector <XVector3D> ret;
    ret.reserve(list.size());

    foreach( const XVector3D &v, list )
        {
        ret << _transform * v;
        }

    return ret;
    }

XVector3D XModeller::transformNormal( XVector3D in )
    {
    return _transform.linear() * in;
    }

XVector <XVector3D> XModeller::transformNormals( const XVector <XVector3D> &list )
    {
    if( _transform.isApprox(XTransform::Identity()) )
        {
        return list;
        }

    XVector <XVector3D> ret;
    ret.reserve(list.size());

    foreach( const XVector3D &v, list )
        {
        ret << _transform.linear() * v;
        }

    return ret;
    }

void XModeller::drawCurve( const XAbstractCurve <XVector3D> &curve, xsize segments )
    {
    xReal start( curve.minimumT() );
    xReal inc( ( curve.maximumT() - curve.minimumT() ) / (segments-1) );

    int begin( _vertex.size() );

    _vertex << transformPoint( curve.sample( start ) );
    _texture << XVector2D();
    _normals << XVector3D();

    for( xsize x=1; x<segments; x++ )
        {
        _linIndices << begin + (x-1) << begin + x;

        _vertex << transformPoint( curve.sample( start + ( x * inc ) ) );

        _texture << XVector2D();
        _normals << XVector3D();
        }
    }
