#include "XTexture.h"
#include "XRenderer.h"
#include "QGLContext"
#include "QImage"

#include "QDebug"

XAbstractTexture::~XAbstractTexture()
  {
  }

XTexture::XTexture( const QImage &im, quint32 opt ) : _texture( im ), _options( opt ), _internal( 0 ), _renderer( 0 )
  {
  }

XTexture::XTexture( const XTexture &cpy ) : _texture( cpy._texture ), _options( cpy._options ), _internal( cpy._internal ), _renderer( 0 )
  {
  /*if(_internal)
    {
    _internal->refCount().ref();
    }*/
  _internal = 0;
  _texture.detach();
  }

void XTexture::clean() const
  {
  if(_internal)
    {
    if(!_internal->refCount().deref())
      {
      xAssert(_renderer);
      _renderer->destroyTexture(_internal);
      }
    _internal = 0;
    }
  }

XTexture::~XTexture( )
  {
  clean();
  }

void XTexture::load( const QImage &im, quint32 opt )
  {
  QMutexLocker l(&_lock);
  clean();

  _texture = im;
  _options = opt;
  }

void XTexture::prepareInternal( XRenderer *r ) const
  {
  QMutexLocker l(&_lock);
  if( !_internal )
    {
    _internal = r->getTexture();
    _renderer = r;
    if( !_texture.isNull() )
      {
      if((_options&InvertY) != false)
        {
        _internal->load( _texture.mirrored() );
        }
      else
        {
        _internal->load( _texture );
        }
      }
    }
  }

XAbstractTexture *XTexture::internal() const
  {
  return _internal;
  }

QDataStream &operator>>( QDataStream &str, XTexture &t )
  {
  t.clean();
  return str >> t._options >> t._texture;
  }

QDataStream &operator<<( QDataStream &str, const XTexture &t )
  {
  return str << t._options << t._texture;
  }
