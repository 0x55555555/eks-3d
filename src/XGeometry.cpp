#include "XGeometry.h"
#include "XRenderer.h"
#include "XTriangle.h"
#include "XCuboid.h"

XAbstractGeometry::~XAbstractGeometry()
  {
  }

XGeometry::XGeometry( BufferType type ) : _internal( 0 ), _changedP( false ), _changedL( false ),
    _changedT( false ), _attributeSizeChanged( false ), _changedAttrs(false), _attributeSize( 0 ), _renderer( 0 ), _type( type )
  {
  }

XGeometry::XGeometry( const XGeometry &cpy ) : _internal( 0 ), _changedP( false ), _changedL( false ),
    _changedT( false ), _renderer( 0 ), _type( cpy._type )
  {
  _attributeSize = cpy._attributeSize;
  _attr1 = cpy._attr1;
  _attr2 = cpy._attr2;
  _attr3 = cpy._attr3;
  _attr4 = cpy._attr4;
  _points = cpy._points;
  _lines = cpy._lines;
  _triangles = cpy._triangles;
  }

XGeometry& XGeometry::operator=( const XGeometry &cpy )
  {
  if( _internal )
    {
    xAssert(_renderer);
    _renderer->destroyGeometry(_internal);
    _internal = 0;
    }

  _attributeSize = cpy._attributeSize;
  _attr1 = cpy._attr1;
  _attr2 = cpy._attr2;
  _attr3 = cpy._attr3;
  _attr4 = cpy._attr4;
  _points = cpy._points;
  _lines = cpy._lines;
  _triangles = cpy._triangles;

  return *this;
  }

const XVector<unsigned int> &XGeometry::points() const
  {
  return _points;
  }

const XVector<unsigned int> &XGeometry::lines() const
  {
  return _lines;
  }

const XVector<unsigned int> &XGeometry::triangles() const
  {
  return _triangles;
  }

const XHash <QString, XVector<xReal> > &XGeometry::attributes1D() const
  {
  return _attr1;
  }

const XHash <QString, XVector<XVector2D> > &XGeometry::attributes2D() const
  {
  return _attr2;
  }

const XHash <QString, XVector<XVector3D> > &XGeometry::attributes3D() const
  {
  return _attr3;
  }

const XHash <QString, XVector<XVector4D> > &XGeometry::attributes4D() const
  {
  return _attr4;
  }

void XGeometry::setPoints( const XVector<unsigned int> &v )
  {
  _points = v;
  _changedP = true;
  }

void XGeometry::setLines( const XVector<unsigned int> &v )
  {
  _lines = v;
  _changedL = true;
  }

void XGeometry::setLines(const xuint32 *in, xsize size)
  {
  _lines.resize(size);
  memcpy(_lines.data(), in, size*sizeof(xuint32));
  _changedL = true;
  }

void XGeometry::setTriangles( const XVector<unsigned int> &v )
  {
  _triangles = v;
  _changedT = true;
  }

void XGeometry::setTriangles(const xuint32 *in, xsize size)
  {
  _triangles.resize(size);
  memcpy(_triangles.data(), in, size*sizeof(xuint32));
  _changedT = true;
  }

void XGeometry::setAttribute( const QString &n, const XVector<xReal> &v )
  {
  if( v.size() )
    {
    _attr1[n] = v;
    if( _attr1[n].size() != _attributeSize )
      {
      _attributeSize = _attr1[n].size();
      _attributeSizeChanged = true;
      }
    }
  else
    {
    _attr1.remove(n);
    }
  _changedA1 << n;
  _changedAttrs = true;
  }

void XGeometry::setAttribute( const QString &n, const XVector<XVector2D> &v )
  {
  if( v.size() )
    {
    _attr2[n] = v;
    if( _attr2[n].size() != _attributeSize )
      {
      _attributeSize = _attr2[n].size();
      _attributeSizeChanged = true;
      }
    }
  else
    {
    _attr1.remove(n);
    }
  _changedA2 << n;
  _changedAttrs = true;
  }

void XGeometry::setAttribute( const QString &n, const XVector<XVector3D> &v )
  {
  if( v.size() )
    {
    _attr3[n] = v;
    if( _attr3[n].size() != _attributeSize )
      {
      _attributeSize = _attr3[n].size();
      _attributeSizeChanged = true;
      }
    }
  else
    {
    _attr1.remove(n);
    }
  _changedA3 << n;
  _changedAttrs = true;
  }

void XGeometry::setAttribute( const QString &n, const XVector<XVector4D> &v )
  {
  if( v.size() )
    {
    _attr4[n] = v;
    if( _attr4[n].size() != _attributeSize )
      {
      _attributeSize = _attr4[n].size();
      _attributeSizeChanged = true;
      }
    }
  else
    {
    _attr1.remove(n);
    }
  _changedA4 << n;
  _changedAttrs = true;
  }

void XGeometry::setAttribute( const QString &n, const xReal *in, xsize size )
  {
  QVector<xReal> vec;
  vec.resize(size);
  memcpy(vec.data(), in, size*sizeof(xReal));
  setAttribute(n, vec);
  }

void XGeometry::setAttribute( const QString &n, const XVector2D *in, xsize size )
  {
  QVector<XVector2D> vec;
  vec.resize(size);
  memcpy(vec.data(), in, size*sizeof(XVector2D));
  setAttribute(n, vec);
  }

void XGeometry::setAttribute( const QString &n, const XVector3D *in, xsize size )
  {
  QVector<XVector3D> vec;
  vec.resize(size);
  memcpy(vec.data(), in, size*sizeof(XVector3D));
  setAttribute(n, vec);
  }

void XGeometry::setAttribute( const QString &n, const XVector4D *in, xsize size )
  {
  QVector<XVector4D> vec;
  vec.resize(size);
  memcpy(vec.data(), in, size*sizeof(XVector4D));
  setAttribute(n, vec);
  }

void XGeometry::removeAttribute( const QString &in )
  {
  if( _attr1.contains(in) )
    {
    _attr1.remove( in );
    _changedA1 << in;
    _attributeSizeChanged = true;
    _changedAttrs = true;
    }
  if( _attr2.contains(in) )
    {
    _attr2.remove( in );
    _changedA2 << in;
    _attributeSizeChanged = true;
    _changedAttrs = true;
    }
  if( _attr3.contains(in) )
    {
    _attr3.remove( in );
    _changedA3 << in;
    _attributeSizeChanged = true;
    _changedAttrs = true;
    }
  if( _attr4.contains(in) )
    {
    _attr4.remove( in );
    _changedA4 << in;
    _attributeSizeChanged = true;
    _changedAttrs = true;
    }
  }

XCuboid XGeometry::computeBounds() const
  {
  const XVector<XVector3D> &vtxList = _attr3["vertex"];

  XCuboid ret;
  foreach( const XVector3D &vtx, vtxList )
    {
    ret.unite(vtx);
    }
  return ret;
  }

void XGeometry::setAttribute( const QString &name, const XList<xReal> &in )
  {
  setAttribute( name, in.toVector() );
  }

void XGeometry::setAttribute( const QString &name, const XList<XVector2D> &in )
  {
  XVector<XVector2D> ver;
  ver.reserve( in.size() );
  foreach( const XVector2D &v, in )
    {
    ver << v;
    }
  setAttribute( name, ver );
  }

void XGeometry::setAttribute( const QString &name, const XList<XVector3D> &in )
  {
  XVector<XVector3D> ver;
  ver.reserve( in.size() );
  foreach( const XVector3D &v, in )
    {
    ver << v;
    }
  setAttribute( name, ver );
  }

void XGeometry::setAttribute( const QString &name, const XList<XVector4D> &in )
  {
  XVector<XVector4D> ver;
  ver.reserve( in.size() );
  foreach( const XVector4D &v, in )
    {
    ver << v;
    }
  setAttribute( name, ver );
  }

void XGeometry::setPoints( const XList<unsigned int> &in )
  {
  setPoints( in.toVector() );
  }

void XGeometry::setLines( const XList<unsigned int> &in )
  {
  setLines( in.toVector() );
  }

void XGeometry::setTriangles( const XList<unsigned int> &in )
  {
  setTriangles( in.toVector() );
  }

void XGeometry::prepareInternal( XRenderer *r ) const
  {
  xAssert(_renderer == 0 || r == _renderer);
  _renderer = r;
  if( !_internal )
    {
    _internal = _renderer->getGeometry( _type );

    _attributeSizeChanged = true;
    _changedAttrs = true;
    _changedP = true;
    _changedL = true;
    _changedT = true;
    }

  if( _changedP )
    {
    _internal->setPoints( _points );
    _changedP = false;
    }
  if( _changedL )
    {
    _internal->setLines( _lines );
    _changedL = false;
    }
  if( _changedT )
    {
    _internal->setTriangles( _triangles );
    _changedT = false;
    }

  if(_changedAttrs)
    {
    if( _attributeSizeChanged )
      {
      _internal->setAttributesSize( _attributeSize, _attr1.size(), _attr2.size(), _attr3.size(), _attr4.size() );

      foreach( QString na, _attr1.keys() )
        {
        _internal->setAttribute( na, _attr1[na] );
        }
      foreach( QString na, _attr2.keys() )
        {
        _internal->setAttribute( na, _attr2[na] );
        }
      foreach( QString na, _attr3.keys() )
        {
        _internal->setAttribute( na, _attr3[na] );
        }
      foreach( QString na, _attr4.keys() )
        {
        _internal->setAttribute( na, _attr4[na] );
        }
      _attributeSizeChanged = false;
      }
    else
      {
      foreach( QString na, _changedA1 )
        {
        if( _attr1.contains( na ) )
          {
          _internal->setAttribute( na, _attr1[na] );
          }
        }
      foreach( QString na, _changedA2 )
        {
        if( _attr2.contains( na ) )
          {
          _internal->setAttribute( na, _attr2[na] );
          }
        }
      foreach( QString na, _changedA3 )
        {
        if( _attr3.contains( na ) )
          {
          _internal->setAttribute( na, _attr3[na] );
          }
        }
      foreach( QString na, _changedA4 )
        {
        if( _attr4.contains( na ) )
          {
          _internal->setAttribute( na, _attr4[na] );
          }
        }
      }
    }

  _changedA1.clear();
  _changedA2.clear();
  _changedA3.clear();
  _changedA4.clear();
  _changedAttrs = 0;
  }

XAbstractGeometry *XGeometry::internal() const
  {
  return _internal;
  }

QDataStream EKS3D_EXPORT &operator<<( QDataStream &s, const XGeometry &geo )
  {
  return s << geo._attr1 << geo._attr2 << geo._attr3 << geo._attr4 << geo._points << geo._lines << geo._triangles;
  }

QDataStream EKS3D_EXPORT &operator>>( QDataStream &s, XGeometry &geo )
  {
  if( geo._internal )
    {
    xAssert(geo._renderer);
    geo._renderer->destroyGeometry(geo._internal);
    geo._internal = 0;
    }

  geo._changedP = true;
  geo._changedL = true;
  geo._changedT = true;
  geo._attributeSizeChanged = true;

  s >> geo._attr1 >> geo._attr2 >> geo._attr3 >> geo._attr4 >> geo._points >> geo._lines >> geo._triangles;

  geo._attributeSize = -1;
  foreach( const XVector<xReal> &vec, geo._attr1 )
    {
    xAssert( geo._attributeSize == -1 || ( geo._attributeSize == vec.size() ) );
    geo._attributeSize = vec.size();
    }
  foreach( const XVector<XVector2D> &vec, geo._attr2 )
    {
    xAssert( geo._attributeSize == -1 || ( geo._attributeSize == vec.size() ) );
    geo._attributeSize = vec.size();
    }
  foreach( const XVector<XVector3D> &vec, geo._attr3 )
    {
    xAssert( geo._attributeSize == -1 || ( geo._attributeSize == vec.size() ) );
    geo._attributeSize = vec.size();
    }
  foreach( const XVector<XVector4D> &vec, geo._attr4 )
    {
    xAssert( geo._attributeSize == -1 || ( geo._attributeSize == vec.size() ) );
    geo._attributeSize = vec.size();
    }

  if(geo._attributeSize == -1)
    {
    xAssert(geo._renderer);
    geo._renderer->destroyGeometry(geo._internal);
    geo._internal = 0;
    }

  return s;
  }

namespace XMeshUtilities
{
bool intersect( QString semantic,
                const XLine &ray,
                const XGeometry &geo,
                XVector3DList *posOut,
                XList <unsigned int> *triOut )
  {
  if(posOut)
    {
    posOut->clear();
    }
  if(triOut)
    {
    triOut->clear();
    }

  xAssert( geo.attributes3D().contains(semantic) );
  const XVector<XVector3D> &positions = geo.attributes3D()[semantic];
  const XVector<unsigned int> &tris = geo.triangles();

  xAssert( tris.size()%3 == 0 );
  unsigned int trianglesSize = tris.size()/3;
  for( unsigned int index=0; index<trianglesSize; ++index )
    {
    const XVector3D &a(positions[tris[index*3]]),
                              &b(positions[tris[(index*3)+1]]),
                              &c(positions[tris[(index*3)+2]]);

    XVector3D pos;
    if( XTriangle(a, b, c).intersects(ray, pos) )
      {
      if(posOut)
        {
        *posOut << pos;
        if(triOut)
          {
          *triOut << index;
          }
        }
      else
        {
        return true;
        }
      }
    }

  return posOut ? posOut->size() > 0 : false;
  }
}
