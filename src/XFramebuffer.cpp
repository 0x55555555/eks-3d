#include "XFramebuffer.h"
#include "XRenderer.h"

XAbstractFramebuffer::~XAbstractFramebuffer()
  {
  }

XFramebuffer::XFramebuffer( )
    : _width( 0 ), _height( 0 ), _options( Colour|Depth ), _colourFormat( RGBA|Byte ), _depthFormat( Byte ), _internal( 0 ), _renderer( 0 )
  {
  }

XFramebuffer::XFramebuffer( int c, int d )
    : _width( 0 ), _height( 0 ), _options( Colour|Depth ), _colourFormat( c ), _depthFormat( d ), _internal( 0 ), _renderer( 0 )
  {
  }

XFramebuffer::XFramebuffer( int options, int c, int d )
    : _width( 0 ), _height( 0 ), _options( options ), _colourFormat( c ), _depthFormat( d ), _internal( 0 ), _renderer( 0 )
  {
  }

XFramebuffer::~XFramebuffer()
  {
  clean();
  }

void XFramebuffer::clean() const
  {
  if( _internal )
    {
    xAssert(_renderer);
    _renderer->destroyFramebuffer(_internal);
    _internal = 0;
    }
  }


void XFramebuffer::setOptions( int o )
  {
  clean();
  _options = o;
  }

void XFramebuffer::setColourFormat( int f )
  {
  clean();
  _colourFormat = f;
  }

void XFramebuffer::setDepthFormat( int f )
  {
  clean();
  _depthFormat = f;
  }

void XFramebuffer::setSize( xuint32 w, xuint32 h )
  {
  clean();
  _width = w;
  _height = h;
  }

XAbstractFramebuffer *XFramebuffer::internal() const
  {
  return _internal;
  }

void XFramebuffer::prepareInternal( XRenderer *r )const
  {
  if( !_internal )
    {
    _renderer = r;
    _internal = r->getFramebuffer( _options, _colourFormat, _depthFormat, _width, _height );
    }
  }
