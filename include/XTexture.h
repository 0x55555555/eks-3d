#ifndef XABSTRACTTEXTURE_H
#define XABSTRACTTEXTURE_H

#include "XRenderer.h"
#include "X3DGlobal.h"
#include "XObject"
#include "XProperty"
#include "QImage"
#include "QMetaType"

class XGLRenderer;
class XRenderer;
class XAbstractTexture;

class EKS3D_EXPORT XTexture
  {
public:
  enum Options
    {
    None = 0,
    InvertY = 1
    };

XProperties:
  XROProperty( QImage, texture );
  XROProperty( quint32, options );

public:
  XTexture( const QImage &image = QImage(), quint32 = None );
  XTexture( const XTexture & );
  ~XTexture( );

  void load( const QImage &, quint32 = None );

  void prepareInternal( XRenderer * ) const;
  XAbstractTexture *internal() const;

  friend QDataStream &operator>>( QDataStream &str, XTexture & );
  friend QDataStream &operator<<( QDataStream &str, const XTexture & );
  
private:
  void clean() const;
  mutable XAbstractTexture *_internal;
  mutable XRenderer *_renderer;
  mutable QMutex _lock;
  };

class EKS3D_EXPORT XAbstractTexture : public XRendererType
  {
public:
  virtual ~XAbstractTexture();
  virtual void load( const QImage & ) = 0;
  virtual QImage save( ) = 0;
  };

Q_DECLARE_METATYPE(XTexture)

#endif // XABSTRACTTEXTURE_H
