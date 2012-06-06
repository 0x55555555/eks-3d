#include "XShape.h"
#include "XShader.h"

class XShapeData : public XSharedData
  {
public:
  XShapeData() : _shader( 0 ), _renderFlags( 0 )
    {
    }
  XShapeData(const XShapeData &cpy)
    {
    _geometry = cpy._geometry;
    _shader = new XShader(*cpy._shader);
    _renderFlags = cpy._renderFlags;
    }
  ~XShapeData()
    {
    delete _shader;
    }

  XGeometry _geometry;
  XShader *_shader;
  int _renderFlags;
  };

XShape::XShape( XGeometry geo, int renderFlags )
  : _d( new XShapeData )
  {
  geometry() = geo;
  setRenderFlags( renderFlags );
  }

XShape::XShape( XGeometry geo, const XShader &shader, int renderFlags )
    : _d( new XShapeData )
  {
  geometry() = geo;
  setShader( shader );
  setRenderFlags( renderFlags );
  }

void XShape::enableRenderFlag( XRenderer::RenderFlags feat )
  {
  _d->_renderFlags |= feat;
  }

void XShape::disableRenderFlag( XRenderer::RenderFlags feat )
  {
  if( ( _d.constPtr()->_renderFlags & feat ) != false )
    {
    _d->_renderFlags -= feat;
    }
  }

void XShape::setRenderFlags( int in )
  {
  _d->_renderFlags = in;
  }

int XShape::renderFlags() const
  {
  return _d.constPtr()->_renderFlags;
  }

XGeometry &XShape::geometry()
  {
  return _d->_geometry;
  }

const XGeometry &XShape::geometry() const
  {
  return _d.constPtr()->_geometry;
  }

void XShape::setShader( const XShader &shader )
  {
  _d->_shader = new XShader( shader );
  }

void XShape::clearShader( )
  {
  delete _d->_shader;
  _d->_shader = 0;
  }

bool XShape::hasShader( ) const
  {
  return _d->_shader;
  }

XShader *XShape::shader()
  {
  return _d->_shader;
  }

const XShader *XShape::constShader() const
  {
  return _d.constPtr()->_shader;
  }

QDataStream &operator<<(QDataStream &stream, const XShape &child)
  {
  bool hasShader = ( child._d.constPtr()->_shader != 0 );
  stream << child._d.constPtr()->_renderFlags
         << child._d.constPtr()->_geometry
         << hasShader;
  if( hasShader )
    {
    //child._d.constPtr()->_shader->save( stream, XShader::Full );
    }
  return stream;
  }

QDataStream &operator>>(QDataStream &stream, XShape &child)
  {
  bool hasShader = false;
  stream >> child._d->_renderFlags
         >> child._d->_geometry
         >> hasShader;
  if( hasShader )
    {
    if( !child._d->_shader )
      {
      child._d->_shader = new XShader();
      }
    //child._d->_shader->restore( stream, XShader::Full );
    }
  else if( child._d->_shader )
    {
    delete child._d->_shader;
    child._d->_shader = 0;
    }
  return stream;
  }
