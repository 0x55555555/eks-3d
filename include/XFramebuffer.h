#ifndef XFRAMEBUFFER_H
#define XFRAMEBUFFER_H

#include "X3DGlobal.h"
#include "XProperty"

#include "QVariant"

class XRenderer;
class XAbstractTexture;

class EKS3D_EXPORT XAbstractFramebuffer
  {
public:
  virtual ~XAbstractFramebuffer( );
  virtual bool isValid() const = 0;
  virtual const XAbstractTexture *colour() const = 0;
  virtual const XAbstractTexture *depth() const = 0;
  };

class XAbstractTexture;

class EKS3D_EXPORT XFramebuffer
  {
public:
  enum Options
    {
    Colour = 1,
    Depth = 2
    };

XProperties:
  XROProperty( xuint32, width );
  XROProperty( xuint32, height );
  XROProperty( xuint32, options );
  XROProperty( int, colourFormat );
  XROProperty( int, depthFormat );

public:
  XFramebuffer( );
  XFramebuffer( int colourFormat, int DepthFormat = Byte );
  XFramebuffer( int options, int colourFormat, int DepthFormat );
  ~XFramebuffer( );

  void setOptions( int options );
  void setColourFormat( int format );
  void setDepthFormat( int format );
  void setSize( xuint32 width, xuint32 height );

  XAbstractFramebuffer *internal() const;
  void prepareInternal( XRenderer * ) const;

private:
  void clean() const;
  mutable XAbstractFramebuffer *_internal;
  mutable XRenderer *_renderer;
  };

#endif // XFRAMEBUFFER_H
