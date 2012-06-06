#ifndef XABSTRACTSHADER_H
#define XABSTRACTSHADER_H

#include "X3DGlobal.h"
#include "XFramebuffer.h"
#include "XProperty"
#include "QByteArray"
#include "XColour"
#include "XVector"
#include "XVector2D"
#include "XVector3D"
#include "XVector4D"
#include "XObject"
#include "QGenericMatrix"
#include "QVariant"

class XRenderer;
class XTexture;
class XGLTexture;
class XShader;
class XAbstractShader;

class EKS3D_EXPORT XAbstractShaderVariable
  {
XProperties:
  XROProperty( XAbstractShader *, abstractShader )

public:
  XAbstractShaderVariable( XAbstractShader * );
  virtual ~XAbstractShaderVariable( );
  virtual void setValue( int value ) = 0;
  virtual void setValue( xReal value ) = 0;
  virtual void setValue( unsigned int value ) = 0;
  virtual void setValue( const XColour &color ) = 0;
  virtual void setValue( const XVector2D &value ) = 0;
  virtual void setValue( const XVector3D &value ) = 0;
  virtual void setValue( const XVector4D &value ) = 0;
  virtual void setValue( const QMatrix2x2 &value ) = 0;
  virtual void setValue( const QMatrix2x3 &value ) = 0;
  virtual void setValue( const QMatrix2x4 &value ) = 0;
  virtual void setValue( const QMatrix3x2 &value ) = 0;
  virtual void setValue( const QMatrix3x3 &value ) = 0;
  virtual void setValue( const QMatrix3x4 &value ) = 0;
  virtual void setValue( const QMatrix4x2 &value ) = 0;
  virtual void setValue( const QMatrix4x3 &value ) = 0;
  virtual void setValue( const QMatrix4x4 &value ) = 0;
  virtual void setValue( const XTexture &value ) = 0;
  virtual void setValueArray( const XVector<int> &values ) = 0;
  virtual void setValueArray( const XVector<xReal> &values ) = 0;
  virtual void setValueArray( const XVector<unsigned int> &values ) = 0;
  virtual void setValueArray( const XVector<XColour> &values ) = 0;
  virtual void setValueArray( const XVector<XVector2D> &values ) = 0;
  virtual void setValueArray( const XVector<XVector3D> &values ) = 0;
  virtual void setValueArray( const XVector<XVector4D> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix2x2> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix2x3> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix2x4> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix3x2> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix3x3> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix3x4> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix4x2> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix4x3> &values ) = 0;
  virtual void setValueArray( const XVector<QMatrix4x4> &values ) = 0;

  virtual void rebind() = 0;
  };

class EKS3D_EXPORT XShaderVariable
  {
XProperties:
  XROProperty( QVariant, value );
  XROProperty( XShader *, shader );
  XROProperty( QString, name );
  XROProperty( XAbstractShaderVariable *, internal );

public:
  XShaderVariable( QString name, XShader *, XAbstractShaderVariable *variable = 0 );
  ~XShaderVariable( );
  void setValue( int value );
  void setValue( xReal value );
  void setValue( unsigned int value );
  void setValue( const XColour &color );
  void setValue( const XVector2D &value );
  void setValue( const XVector3D &value );
  void setValue( const XVector4D &value );
  void setValue( const QMatrix2x2 &value );
  void setValue( const QMatrix2x3 &value );
  void setValue( const QMatrix2x4 &value );
  void setValue( const QMatrix3x2 &value );
  void setValue( const QMatrix3x3 &value );
  void setValue( const QMatrix3x4 &value );
  void setValue( const QMatrix4x2 &value );
  void setValue( const QMatrix4x3 &value );
  void setValue( const QMatrix4x4 &value );
  void setValue( const XTexture &value );
  void setValueArray( const XVector<int> &values );
  void setValueArray( const XVector<xReal> &values );
  void setValueArray( const XVector<unsigned int> &values );
  void setValueArray( const XVector<XColour> &values );
  void setValueArray( const XVector<XVector2D> &values );
  void setValueArray( const XVector<XVector3D> &values );
  void setValueArray( const XVector<XVector4D> &values );
  void setValueArray( const XVector<QMatrix2x2> &values );
  void setValueArray( const XVector<QMatrix2x3> &values );
  void setValueArray( const XVector<QMatrix2x4> &values );
  void setValueArray( const XVector<QMatrix3x2> &values );
  void setValueArray( const XVector<QMatrix3x3> &values );
  void setValueArray( const XVector<QMatrix3x4> &values );
  void setValueArray( const XVector<QMatrix4x2> &values );
  void setValueArray( const XVector<QMatrix4x3> &values );
  void setValueArray( const XVector<QMatrix4x4> &values );

  void setVariantValue( const QVariant & );

  void prepareInternal( );
  };

class EKS3D_EXPORT XAbstractShader
  {
XProperties:
  XROProperty( XRenderer *, renderer );

public:
  XAbstractShader( XRenderer * );
  virtual ~XAbstractShader();

  enum ComponentType
    {
    Fragment,
    Vertex,
    Geometry
    };
  virtual bool addComponent(ComponentType c, const QString& source, QStringList &log) = 0;
  virtual bool build(QStringList &log) = 0;
  virtual bool isValid() = 0;

  virtual XAbstractShaderVariable *createVariable( QString, XAbstractShader * ) = 0;
  virtual void destroyVariable( XAbstractShaderVariable * ) = 0;

private:
  mutable QAtomicInt _ref;
  };

Q_DECLARE_METATYPE(XShader)

class EKS3D_EXPORT XShaderComponent
  {
XProperties:
  XROProperty( XRenderer *, renderer );

public:
  };

class EKS3D_EXPORT XShader
  {
XProperties:
  XROProperty( XRenderer *, renderer );
  XRORefProperty( QStringList, log );

public:
  XShader();
  XShader( const XShader & );
  ~XShader();

  void addComponent(XAbstractShader::ComponentType, const QString &source);
  void clear();

  XShaderVariable *getVariable(const QString &type);

  void setToDefinedType(const QString &type);

  QHash <QString, XShaderVariable*> variables() const;

  void prepareInternal( XRenderer * ) const;
  XAbstractShader *internal() const;

private:
  QHash <QString, XShaderVariable*> _variables;

  struct Component
    {
    XAbstractShader::ComponentType type;
    QString source;
    };
  XList<Component> _components;

  mutable XAbstractShader *_internal;
  };

inline bool operator==(const XShader&, const XShader&)
  {
  xAssertFail();
  return false;
  }

inline bool operator!=(const XShader&, const XShader&)
  {
  xAssertFail();
  return false;
  }

inline bool operator<<(const QTextStream&, const XShader&)
  {
  xAssertFail();
  return false;
  }

inline bool operator<<(const QDataStream&, const XShader&)
  {
  xAssertFail();
  return false;
  }

inline bool operator>>(QTextStream&, XShader&)
  {
  xAssertFail();
  return false;
  }

inline bool operator>>(QDataStream&, XShader&)
  {
  xAssertFail();
  return false;
  }

#endif // XABSTRACTSHADER_H
