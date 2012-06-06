#include "XShader.h"
#include "XRenderer.h"
#include "XTexture.h"
#include "QGLShaderProgram"
#include "XRenderer.h"
#include "QVariant"
#include "QVector2D"
#include "QVector3D"
#include "QVector4D"
#include "QStringList"
#include "QFile"

template <typename T> XVector <T> toVector( const QVariantList &variantList )
  {
  XVector <T> ret;
  foreach( const QVariant &variant, variantList )
    {
    ret << variant.value<T>();
    }
  return ret;
  }

template <typename T> QVariantList toList( const QVector <T> &vector )
  {
  QVariantList ret;
  foreach( const T &var, vector )
    {
    ret << QVariant::fromValue<T>( var );
    }
  return ret;
  }

XAbstractShaderVariable::XAbstractShaderVariable( XAbstractShader *s ) : _abstractShader( s )
  {
  }

XAbstractShaderVariable::~XAbstractShaderVariable( )
  {
  }

XShaderVariable::XShaderVariable( QString n, XShader *s, XAbstractShaderVariable *v )
    : _shader( s ), _name( n ), _internal( v )
  {
  }

XShaderVariable::~XShaderVariable( )
  {
  XAbstractShader *sInt = _shader->internal();
  if(sInt && _internal)
    {
    sInt->destroyVariable(_internal);
    }
  }

void XShaderVariable::setValue( int value )
  {
  _value = value;
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( xReal value )
  {
  _value = value;
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( unsigned int value )
  {
  _value = value;
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const XColour &value )
  {
  _value.setValue((XVector4D&)value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const XVector2D &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const XVector3D &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const XVector4D &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix2x2 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix2x3 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix2x4 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix3x2 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix3x3 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix3x4 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix4x2 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix4x3 &value )
  {
  _value.setValue(value);
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const QMatrix4x4 &value )
  {
  _value = value;
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValue( const XTexture &value )
  {
  _value.setValue( value );
  _internal ? _internal->setValue( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<int> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<xReal> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<unsigned int> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<XColour> &value )
  {
  _value = toList( reinterpret_cast<const XVector<XVector4D> &>(value) );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<XVector2D> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<XVector3D> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<XVector4D> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix2x2> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix2x3> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix2x4> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix3x2> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix3x3> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix3x4> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix4x2> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix4x3> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setValueArray( const XVector<QMatrix4x4> &value )
  {
  _value = toList( value );
  _internal ? _internal->setValueArray( value ) : xNoop();
  }

void XShaderVariable::setVariantValue( const QVariant &value )
  {
  _value = value;
  if( _internal )
    {
    if( value.type() == QVariant::Int )
      setValue( value.toInt() );
    else if( value.type() == QVariant::Double )
      setValue( value.toFloat() );
    else if( value.type() == QVariant::UInt )
      setValue( value.toUInt() );
    else if( value.canConvert<XVector2D>() )
      setValue( value.value<XVector2D>() );
    else if( value.canConvert<XVector3D>())
      setValue( value.value<XVector3D>() );
    else if( value.canConvert<XVector4D>() )
      setValue( value.value<XVector4D>() );
    else if( value.canConvert<QMatrix2x2>() )
      setValue( value.value<QMatrix2x2>() );
    else if( value.canConvert<QMatrix2x3>() )
      setValue( value.value<QMatrix2x3>() );
    else if( value.canConvert<QMatrix2x4>() )
      setValue( value.value<QMatrix2x4>() );
    else if( value.canConvert<QMatrix3x2>() )
      setValue( value.value<QMatrix4x2>() );
    else if( value.canConvert<QMatrix3x3>() )
      setValue( value.value<QMatrix3x3>() );
    else if( value.canConvert<QMatrix3x4>() )
      setValue( value.value<QMatrix3x4>() );
    else if( value.canConvert<QMatrix4x2>() )
      setValue( value.value<QMatrix4x2>() );
    else if( value.canConvert<QMatrix4x3>() )
      setValue( value.value<QMatrix4x3>() );
    else if( value.canConvert<QMatrix4x4>() )
      setValue( value.value<QMatrix4x4>() );
    else if( value.canConvert<XTexture>() )
      setValue( value.value<XTexture>() );
    else if( value.type() == QVariant::List )
      {
      QVariantList list = value.toList();
      if( list.size() == 0 )
        {
        return;
        }
      if( list.front().type() == QVariant::Int )
        setValueArray( toVector<int>(list) );
      else if( list.front().type() == QVariant::Double )
        setValueArray( toVector<float>(list) );
      else if( list.front().type() == QVariant::UInt )
        setValueArray( toVector<unsigned int>(list) );
      else if( list.front().canConvert<XVector2D>() )
        setValueArray( toVector<XVector2D>(list) );
      else if( list.front().canConvert<XVector3D>() )
        setValueArray( toVector<XVector3D>(list) );
      else if( list.front().canConvert<XVector4D>() )
        setValueArray( toVector<XVector4D>(list) );
      else if( list.front().canConvert<QMatrix2x2>() )
        setValueArray( toVector<QMatrix2x2>(list) );
      else if( list.front().canConvert<QMatrix2x3>() )
        setValueArray( toVector<QMatrix2x3>(list) );
      else if( list.front().canConvert<QMatrix2x4>() )
        setValueArray( toVector<QMatrix2x4>(list) );
      else if( list.front().canConvert<QMatrix3x2>() )
        setValueArray( toVector<QMatrix3x2>(list) );
      else if( list.front().canConvert<QMatrix3x3>() )
        setValueArray( toVector<QMatrix3x3>(list) );
      else if( list.front().canConvert<QMatrix3x4>() )
        setValueArray( toVector<QMatrix3x4>(list) );
      else if( list.front().canConvert<QMatrix4x2>() )
        setValueArray( toVector<QMatrix4x2>(list) );
      else if( list.front().canConvert<QMatrix4x3>() )
        setValueArray( toVector<QMatrix4x3>(list) );
      else if( list.front().canConvert<QMatrix4x4>() )
        setValueArray( toVector<QMatrix4x4>(list) );
      }
    }
  }

void XShaderVariable::prepareInternal( )
  {
  if( !_internal )
    {
    xAssert( _shader->internal() );
    _internal = _shader->internal()->createVariable( _name, _shader->internal() );
    setVariantValue( _value );
    }
  }

XAbstractShader::XAbstractShader( XRenderer *r ) : _renderer( r )
  {
  }

XAbstractShader::~XAbstractShader()
  {
  }

XShader::XShader() : _renderer( 0 ), _internal( 0 )
  {
  }

XShader::XShader( const XShader &c ) : _renderer( 0 ), _components(c._components), _internal( 0 )
  {
  foreach( QString n, c._variables.keys() )
    {
    XShaderVariable *var = getVariable( n );
    var->setVariantValue( c._variables.value(n)->value() );
    }
  }

XShader::~XShader()
  {
  clear();
  }


void XShader::addComponent(XAbstractShader::ComponentType t, const QString &source)
  {
  Component c;
  c.source = source;
  c.type = t;
  _components << c;

  delete _internal;
  _internal = 0;
  }

void XShader::clear()
  {
  _components.clear();

  foreach( XShaderVariable *var, _variables )
    {
    delete var;
    }
  _variables.clear();

  delete _internal;
  _internal = 0;
  }

XShaderVariable *XShader::getVariable(const QString &in )
  {
  if( _variables.contains( in ) )
    {
    return _variables[in];
    }

  XAbstractShaderVariable *internalVariable = 0;
  if( _internal )
    {
    internalVariable = ( _internal->createVariable( in, _internal ) );
    }

  XShaderVariable *ret = new XShaderVariable( in, this, internalVariable );
  _variables.insert( in, ret );
  return ret;
  }

void XShader::setToDefinedType(const QString &type)
  {
  clear();

  QFile v(":/GLResources/shaders/" + type + ".vert");
  QFile f(":/GLResources/shaders/" + type + ".frag");
  if(v.open(QIODevice::ReadOnly) && f.open(QIODevice::ReadOnly))
    {
    addComponent(XAbstractShader::Vertex, v.readAll());
    addComponent(XAbstractShader::Fragment, f.readAll());
    }
  }

QHash <QString, XShaderVariable*> XShader::variables() const
  {
  return _variables;
  }

void XShader::prepareInternal( XRenderer *renderer ) const
  {
  if( !_internal )
    {
    if(_components.isEmpty())
      {
      qWarning() << "Empty shader used, setting to default";
      ((XShader*)this)->setToDefinedType("default");
      }

    _internal = renderer->getShader( );
    xAssert( _internal );

    XShader *logAccess((XShader*)this);
    logAccess->_log.clear();
    foreach(const Component &c, _components)
      {
      bool result = _internal->addComponent(c.type, c.source, logAccess->_log);

      if(!result)
        {
        logAccess->_log << "Shader component failed to compile";
        }
      }
    bool result = _internal->build(logAccess->_log);

    if(!result)
      {
      logAccess->_log << "Shader failed to build";
      }

    if(!_internal->isValid())
      {
      logAccess->_log << "Shader was not valid for use after building.";
      }

    foreach( XShaderVariable *var, _variables )
      {
      var->prepareInternal();
      var->internal()->rebind();
      }
    }
  }

XAbstractShader *XShader::internal( ) const
  {
  return _internal;
  }
