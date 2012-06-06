#include "XGLRenderer.h"
#include "GL/glew.h"
#include "XFramebuffer.h"
#include "XGeometry.h"
#include "XShader.h"
#include "XTexture.h"
#include "QVariant"
#include "QGLShaderProgram"
#include "QDebug"
#include "XShader.h"
#include "QFile"

const char *glErrorString( int err )
    {
    if( err == GL_INVALID_ENUM )
        {
        return "GL Error: Invalid Enum";
        }
    else if( err == GL_INVALID_VALUE )
        {
        return "GL Error: Invalid Value";
        }
    else if( err == GL_INVALID_OPERATION )
        {
        return "GL Error: Invalid Operation";
        }
    else if( err == GL_STACK_OVERFLOW )
        {
        return "GL Error: Stack Overflow";
        }
    else if( err == GL_STACK_UNDERFLOW )
        {
        return "GL Error: Stack Underflow";
        }
    else if( err == GL_OUT_OF_MEMORY )
        {
        return "GL Error: Out Of Memory";
        }
    return "GL Error: No Error";
    }

#if 1
# define GLE ; { int _GL = glGetError(); xAssertMessage(!_GL, "GL Error", _GL, glErrorString( _GL )); }
# define GLE_QUIET ; glGetError()
#else
# define GLE ; { int _GL = glGetError(); if( _GL ) { qCritical() << __FILE__ << __LINE__<< glErrorString( _GL ); } }
# define GLE_QUIET ; glGetError()
#endif

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------

class XGLTexture : public XAbstractTexture
  {
public:
  XGLTexture( XGLRenderer * );
  XGLTexture( XGLRenderer *, int format, int width, int height );
  ~XGLTexture();
  virtual void load( const QImage & );
  virtual QImage save( );

private:
  void clear();
  XGLRenderer *_renderer;
  unsigned int _id;
  friend class XGLFramebuffer;
  friend class XGLShaderVariable;
  friend class XGLRenderer;
  };

//----------------------------------------------------------------------------------------------------------------------
// FRAMEBUFFER
//----------------------------------------------------------------------------------------------------------------------

class XGLFramebuffer : public XAbstractFramebuffer
    {
public:
    XGLFramebuffer( XGLRenderer *, int options, int colourFormat, int depthFormat, int width, int height );
    ~XGLFramebuffer( );

    void bind();
    void unbind();

    virtual bool isValid() const;

    virtual const XAbstractTexture *colour() const;
    virtual const XAbstractTexture *depth() const;

private:
    XGLRenderer *_renderer;
    XGLTexture *_colour;
    XGLTexture *_depth;
    unsigned int _buffer;
    friend class XGLShaderVariable;
    friend class XGLRenderer;
    };

//----------------------------------------------------------------------------------------------------------------------
// SHADER
//----------------------------------------------------------------------------------------------------------------------

class XGLShader : public XAbstractShader
    {
public:
    XGLShader( XGLRenderer * );
    ~XGLShader();

private:
    virtual bool addComponent(ComponentType c, const QString &source, QStringList &log);
    virtual bool build(QStringList &log);
    virtual bool isValid();

    virtual XAbstractShaderVariable *createVariable( QString, XAbstractShader * );
    virtual void destroyVariable( XAbstractShaderVariable * );

    QGLShaderProgram shader;
    friend class XGLRenderer;
    friend class XGLShaderVariable;
    };

//----------------------------------------------------------------------------------------------------------------------
// SHADER VARIABLE
//----------------------------------------------------------------------------------------------------------------------

#define GL_SHADER_VARIABLE_PARENT static_cast<XGLShader*>(abstractShader())

class XGLShaderVariable : public XAbstractShaderVariable
    {
public:
    XGLShaderVariable( XAbstractShader *, QString );
    ~XGLShaderVariable( );

    void setValue( int value );
    void setValue( xReal value );
    void setValue( unsigned int value );
    void setValue( const XColour &value );
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

    virtual void rebind();

    inline void bindShader()
      {
      XRenderer *r = GL_SHADER_VARIABLE_PARENT->renderer();
      if(static_cast<XGLRenderer*>(r)->_currentShader != GL_SHADER_VARIABLE_PARENT)
        {
        static_cast<XGLRenderer*>(r)->_currentShader = GL_SHADER_VARIABLE_PARENT;
        GL_SHADER_VARIABLE_PARENT->shader.bind();
        }
      }

private:
    void clear();
    QString _name;
    int _location;
    XTexture *_texture;
    QVariant _value;
    friend class XGLRenderer;
    };

//----------------------------------------------------------------------------------------------------------------------
// GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------

class XGLGeometryCache : public XAbstractGeometry
    {
public:
    XGLGeometryCache( XGLRenderer *, XGeometry::BufferType );
    ~XGLGeometryCache( );

    virtual void setPoints( const XVector<unsigned int> & );
    virtual void setLines( const XVector<unsigned int> & );
    virtual void setTriangles( const XVector<unsigned int> & );

    virtual void setAttributesSize( int, int, int, int, int );

    virtual void setAttribute( QString, const XVector<xReal> & );
    virtual void setAttribute( QString, const XVector<XVector2D> & );
    virtual void setAttribute( QString, const XVector<XVector3D> & );
    virtual void setAttribute( QString, const XVector<XVector4D> & );

private:
    unsigned int _vertexArray;

    unsigned int _pointArray;
    unsigned int _lineArray;
    unsigned int _triangleArray;
    unsigned int _pointSize;
    unsigned int _lineSize;
    unsigned int _triangleSize;

    int _type;

    int getCacheOffset( const QString &name, int components, int attrSize );
    struct DrawCache
        {
        QString name;
        int components;
        int offset;
        };
    XList <DrawCache> _cache;
    int _usedCacheSize;

    XGLRenderer *_renderer;
    friend class XGLRenderer;
    };

//----------------------------------------------------------------------------------------------------------------------
// RENDERER
//----------------------------------------------------------------------------------------------------------------------


XGLRenderer::XGLRenderer() : _context(0), _currentShader(0), _currentFramebuffer(0)
  {
  m_ids.reserve(8);
  }

void XGLRenderer::setContext(QGLContext *ctx)
  {
  _context = ctx;
  }

QGLContext *XGLRenderer::context()
    {
    return _context;
    }

const QGLContext *XGLRenderer::context() const
    {
    return _context;
    }

void XGLRenderer::intialise()
    {
    glewInit() GLE;
    glEnable( GL_DEPTH_TEST ) GLE;
    }

void XGLRenderer::pushTransform( const XTransform &trans )
    {
    glMatrixMode( GL_MODELVIEW ) GLE;
    glPushMatrix() GLE;
    glMultMatrixf( trans.data() ) GLE;
    }

void XGLRenderer::popTransform( )
    {
    glMatrixMode( GL_MODELVIEW ) GLE;
    glPopMatrix() GLE;
    }

void XGLRenderer::clear( int c )
    {
    int realMode = ((c&ClearColour) != false) ? GL_COLOR_BUFFER_BIT : 0;
    realMode |= ((c&ClearDepth) != false) ? GL_DEPTH_BUFFER_BIT : 0;
    glClear( realMode ) GLE;
    }

void XGLRenderer::debugRenderLocator(DebugLocatorMode m)
  {
  if((m&ClearShader) != 0)
    {
    _currentShader = 0;
    glUseProgram(0);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }

  glBegin(GL_LINES);
    glVertex3f(-0.5, 0, 0);
    glVertex3f(0.5, 0, 0);
    glVertex3f(0, -0.5, 0);
    glVertex3f(0, 0.5, 0);
    glVertex3f(0, 0, -0.5);
    glVertex3f(0, 0, 0.5);
  glEnd();
  }

void XGLRenderer::enableRenderFlag( RenderFlags f )
    {
    if( f == AlphaBlending )
        {
        glEnable( GL_BLEND ) GLE;
        glBlendFunc(GL_SRC_ALPHA,GL_ONE) GLE;
        }
    else if( f == DepthTest )
        {
        glEnable( GL_DEPTH_TEST ) GLE;
        }
    else if( f == BackfaceCulling )
        {
        glEnable( GL_CULL_FACE ) GLE;
        }
    }

void XGLRenderer::disableRenderFlag( RenderFlags f )
    {
    if( f == AlphaBlending )
        {
        glDisable( GL_BLEND ) GLE;
        }
    else if( f == DepthTest )
        {
        glDisable( GL_DEPTH_TEST ) GLE;
        }
    else if( f == BackfaceCulling )
        {
        glDisable( GL_CULL_FACE ) GLE;
        }
    }

void XGLRenderer::setViewportSize( QSize size )
    {
    _size = size;
    glViewport( 0, 0, size.width(), size.height() ) GLE;
    }

void XGLRenderer::setProjectionTransform( const XComplexTransform &trans )
    {
    glMatrixMode( GL_PROJECTION ) GLE;
    glLoadIdentity() GLE;
    glMultMatrixf( trans.data() ) GLE;
    glMatrixMode( GL_MODELVIEW ) GLE;
    }

QDebug operator<<( QDebug dbg, XVector2D v )
    {
    return dbg << "Vertex2D(" << v.x() << "," << v.y() << ")";
    }

QDebug operator<<( QDebug dbg, XVector3D v )
    {
    return dbg << "Vertex2D(" <<v.x() << "," << v.y() << "," << v.z() << ")";
    }

void XGLRenderer::drawGeometry( const XGeometry &cache )
  {
  if( _currentShader )
    {
    cache.prepareInternal( this );
    XGLGeometryCache *gC = static_cast<XGLGeometryCache*>((&cache)->internal());

    glBindBuffer( GL_ARRAY_BUFFER, gC->_vertexArray ) GLE;

    m_ids.clear();
    foreach( const XGLGeometryCache::DrawCache &ref, gC->_cache )
      {
      int location( _currentShader->shader.attributeLocation(ref.name) );
      if( location >= 0 )
        {
        m_ids << location;
        glEnableVertexAttribArray( location ) GLE;
        glVertexAttribPointer( location, ref.components, GL_FLOAT, GL_FALSE, 0, (GLvoid*)ref.offset ) GLE;
        }
      }

    if( gC->_pointArray )
      {
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gC->_pointArray ) GLE;
      glDrawElements( GL_POINTS, gC->_pointSize, GL_UNSIGNED_INT, (GLvoid*)((char*)NULL)) GLE;
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;
      }

    if( gC->_lineArray )
      {
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gC->_lineArray ) GLE;
      glDrawElements( GL_LINES, gC->_lineSize, GL_UNSIGNED_INT, (GLvoid*)((char*)NULL)) GLE;
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;
      }

    if( gC->_triangleArray )
      {
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gC->_triangleArray ) GLE;
      glDrawElements( GL_TRIANGLES, gC->_triangleSize, GL_UNSIGNED_INT, (GLvoid*)((char*)NULL)) GLE;
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;
      }

    foreach( int id, m_ids )
      {
      glDisableVertexAttribArray( id ) GLE;
      }

    glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
    }
  }

XAbstractGeometry *XGLRenderer::getGeometry( XGeometry::BufferType t )
    {
    return new XGLGeometryCache( this, t );
    }

void XGLRenderer::setShader( const XShader *shader )
  {
  if( shader && ( _currentShader == 0 || _currentShader != shader->internal() ) )
    {
    shader->prepareInternal( this );
    _currentShader = static_cast<XGLShader*>(shader->internal());

    if(_currentShader->shader.isLinked())
      {
      _currentShader->shader.bind() GLE;

      int x=0;
      foreach( XShaderVariable *var, shader->variables() )
        {
        XGLShaderVariable *glVar( static_cast<XGLShaderVariable*>(var->internal()) );
        if( glVar->_texture )
          {
          const XTexture *tex( glVar->_texture );
          tex->prepareInternal( this );
          const XGLTexture *glTex( static_cast<const XGLTexture*>(tex->internal()) );
          xAssert( glTex );
          glActiveTexture( GL_TEXTURE0 + x ) GLE;
          glBindTexture( GL_TEXTURE_2D, glTex->_id ) GLE;
          _currentShader->shader.setUniformValue( glVar->_location, x );
          }
        x++;
        }
      }
    else
      {
      _currentShader = 0;
      }
    }
  else if( shader == 0 && _currentShader != 0 )
    {
    _currentShader->shader.release() GLE;
    _currentShader = 0;
    }
  }

XAbstractShader *XGLRenderer::getShader( )
  {
  return new XGLShader( this );
  }

void XGLRenderer::setFramebuffer( const XFramebuffer *fb )
  {
  if( _currentFramebuffer )
    {
    _currentFramebuffer->unbind();
    }

  if( fb )
    {
    fb->prepareInternal( this );
    _currentFramebuffer = static_cast<XGLFramebuffer*>(fb->internal());
    }

  if( _currentFramebuffer )
    {
    _currentFramebuffer->bind();
    }
  }

XAbstractFramebuffer *XGLRenderer::getFramebuffer( int options, int c, int d, int width, int height )
  {
  return new XGLFramebuffer( this, options, c, d, width, height );
  }

XAbstractTexture *XGLRenderer::getTexture()
  {
  return new XGLTexture( this );
  }

QSize XGLRenderer::viewportSize()
  {
  return _size;
  }

void XGLRenderer::destroyShader( XAbstractShader *shader )
  {
  delete shader;
  }

void XGLRenderer::destroyGeometry( XAbstractGeometry *geometry )
  {
  delete geometry;
  }

void XGLRenderer::destroyTexture( XAbstractTexture *texture )
  {
  delete texture;
  }

void XGLRenderer::destroyFramebuffer( XAbstractFramebuffer *fb )
  {
  delete fb;
  }

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------

int getFormat( int format )
  {
  if( (format&RGBA) != false )
    {
    return GL_RGBA;
    }
  else if( (format&RGB) != false )
    {
    return GL_RGB;
    }
  else if( (format&Short) != false )
    {
    return GL_DEPTH_COMPONENT;
    }
  else if( (format&Float) != false )
    {
    return GL_DEPTH_COMPONENT;
    }
  return GL_RGBA;
  }

int getInternalFormat( int format )
  {
  switch( format )
    {
  case RGBA|Byte:
    return GL_RGBA8;
  case RGBA|Half:
    return GL_RGBA16F_ARB;
  case RGBA|Float:
    return GL_RGBA32F_ARB;
  case RGB|Byte:
    return GL_RGB8;
  case RGB|Half:
    return GL_RGBA16F_ARB;
  case RGB|Float:
    return GL_RGB32F_ARB;
  case Short:
    return GL_DEPTH_COMPONENT16;
  case Float:
    return GL_DEPTH_COMPONENT32F_NV;
  default:
      qDebug() << "Invalid format option" << format;
    }
  return GL_RGBA8;
  }

XGLTexture::XGLTexture( XGLRenderer *r ) : _renderer( r ), _id( 0 )
  {
  }

XGLTexture::XGLTexture( XGLRenderer *r, int format, int width, int height ) : _renderer( r )
  {
  glGenTextures( 1, &_id ) GLE;
  glBindTexture( GL_TEXTURE_2D, _id ) GLE;

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ) GLE;
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) GLE;

  // could also be GL_REPEAT
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) GLE;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) GLE;

  // 0 at end could be data to unsigned byte...
  glTexImage2D( GL_TEXTURE_2D, 0, getInternalFormat( format ), width, height, 0, getFormat( format ), GL_UNSIGNED_BYTE, (const GLvoid *)0 ) GLE;

  glBindTexture( GL_TEXTURE_2D, 0 ) GLE;
  }

XGLTexture::~XGLTexture()
  {
  clear();
  }

void XGLTexture::load( const QImage &im )
  {
  clear();
  _id = _renderer->context()->bindTexture( im ) GLE;
  }

QImage XGLTexture::save( )
  {
  if( _id != 0 )
    {
    int width, height;
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width ) GLE;
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height ) GLE;
    QImage ret( QSize( width, height ), QImage::Format_ARGB32_Premultiplied );
    glGetTexImage( GL_TEXTURE_2D, 0, GL_BGRA, GL_BYTE, ret.bits() ) GLE;
    return ret;
    }
  return QImage();
  }

void XGLTexture::clear()
  {
  _renderer->context()->deleteTexture( _id ) GLE;
  }

//----------------------------------------------------------------------------------------------------------------------
// FRAMEBUFFER
//----------------------------------------------------------------------------------------------------------------------

XGLFramebuffer::XGLFramebuffer( XGLRenderer *r, int options, int cF, int dF, int width, int height )
    : _renderer( r ), _colour( 0 ), _depth( 0 )
  {
  glGenFramebuffersEXT( 1, &_buffer ) GLE;
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, _buffer ) GLE;

  if( width <= 0 || height <= 0 )
    {
    width = r->viewportSize().width();
    height = r->viewportSize().height();
    }

  if( (options&XFramebuffer::Colour) != false )
    {
    _colour = new XGLTexture( r, cF, width, height ) GLE;
    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _colour->_id, 0 ) GLE;
    }

  if( (options&XFramebuffer::Depth) != false )
    {
    _depth = new XGLTexture( r, dF, width, height ) GLE;
    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, _depth->_id, 0 ) GLE;
    }

  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 ) GLE;
  xAssert( isValid() );
  }

XGLFramebuffer::~XGLFramebuffer( )
  {
  delete _colour;
  delete _depth;
  if( _buffer )
    {
    glDeleteFramebuffersEXT( 1, &_buffer ) GLE;
    }
  }

bool XGLFramebuffer::isValid() const
  {
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, _buffer ) GLE;
  int status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) GLE;

  if( status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT )
    {
    qWarning() << "Framebuffer Incomplete attachment";
    }
  else if( status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT )
    {
    qWarning() << "Framebuffer Incomplete dimensions";
    }
  else if( status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT )
    {
    qWarning() << "Framebuffer Incomplete missing attachment";
    }
  else if( status == GL_FRAMEBUFFER_UNSUPPORTED_EXT )
    {
    qWarning() << "Framebuffer unsupported attachment";
    }

  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 ) GLE;

  return status == GL_FRAMEBUFFER_COMPLETE_EXT;
  }

void XGLFramebuffer::bind()
  {
  xAssert( isValid() );
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, _buffer ) GLE;
  }

void XGLFramebuffer::unbind()
  {
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 ) GLE;
  }

const XAbstractTexture *XGLFramebuffer::colour() const
  {
  return _colour;
  }

const XAbstractTexture *XGLFramebuffer::depth() const
  {
  return _depth;
  }

//----------------------------------------------------------------------------------------------------------------------
// SHADER
//----------------------------------------------------------------------------------------------------------------------

XGLShader::XGLShader( XGLRenderer *renderer ) : XAbstractShader( renderer ), shader( renderer->context() )
  {
  }

XGLShader::~XGLShader()
  {
  XGLRenderer *r = static_cast<XGLRenderer*>(renderer());
  if(r && r->_currentShader == this)
    {
    r->_currentShader = 0;
    shader.release();
    }
  }

bool XGLShader::addComponent(ComponentType c, const QString &source, QStringList &log)
  {
  QGLShader::ShaderTypeBit t = QGLShader::Fragment;
  if(c == Vertex)
    {
    t = QGLShader::Vertex;
    }
  else if(c == Geometry)
    {
    t = QGLShader::Geometry;
    }

  QGLShader *component = new QGLShader(t, &shader) GLE;

  bool result = component->compileSourceCode(source) GLE;

  if(result)
    {
    shader.addShader(component) GLE;
    }

  QString logEntry = component->log();
  if(!logEntry.isEmpty())
    {
    log << logEntry;
    }

  if(!result)
    {
    delete component;
    }

  return result;
  }

bool XGLShader::build(QStringList &log)
  {
  bool result = shader.link() GLE;

  QString logEntry = shader.log();
  if(!logEntry.isEmpty())
    {
    log << logEntry;
    }

  return result;
  }

bool XGLShader::isValid()
  {
  bool result = shader.isLinked() GLE;
  return result;
  }

XAbstractShaderVariable *XGLShader::createVariable( QString in, XAbstractShader *s )
  {
  return new XGLShaderVariable( s, in );
  }

void XGLShader::destroyVariable( XAbstractShaderVariable *var )
  {
  delete var;
  }

//----------------------------------------------------------------------------------------------------------------------
// SHADER VARIABLE
//----------------------------------------------------------------------------------------------------------------------


XGLShaderVariable::XGLShaderVariable( XAbstractShader *s, QString name )
    : XAbstractShaderVariable( s ), _name( name ), _texture( 0 )
  {
  rebind();
  }

XGLShaderVariable::~XGLShaderVariable( )
  {
  delete _texture;
  }

void XGLShaderVariable::setValue( int value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( xReal value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( unsigned int value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const XColour &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, toQt(value) ) GLE;
  }

void XGLShaderVariable::setValue( const XVector2D &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, toQt(value) ) GLE;
  }

void XGLShaderVariable::setValue( const XVector3D &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, toQt(value) ) GLE;
  }

void XGLShaderVariable::setValue( const XVector4D &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, toQt(value) ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix2x2 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix2x3 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix2x4 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix3x2 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix3x3 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix3x4 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix4x2 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix4x3 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const QMatrix4x4 &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( const XTexture &value )
  {
  clear();
  bindShader();
  _texture = new XTexture( value );
  _texture->prepareInternal( abstractShader()->renderer() ) GLE; // todo, use texture units.
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, (int)0) GLE; // static_cast<XGLTexture*>(_texture->internal())->_id) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<int> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<xReal> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size(), 1 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<unsigned int> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<XColour> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, values.front().data(), values.size(), 4 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<XVector2D> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, values.front().data(), values.size(), 2 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<XVector3D> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, values.front().data(), values.size(), 3 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<XVector4D> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, values.front().data(), values.size(), 4 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix2x2> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix2x3> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix2x4> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix3x2> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix3x3> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix3x4> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix4x2> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix4x3> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<QMatrix4x4> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::rebind()
  {
  _location = GL_SHADER_VARIABLE_PARENT->shader.uniformLocation( _name ) GLE;
  }

void XGLShaderVariable::clear()
  {
  if( _texture )
    {
    delete _texture;
    }
  _texture = 0;
  }

#undef GL_SHADER_VARIABLE_PARENT

//----------------------------------------------------------------------------------------------------------------------
// GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------

XGLGeometryCache::XGLGeometryCache( XGLRenderer *r, XGeometry::BufferType type ) : _renderer( r )
  {
  _pointArray = 0;
  _lineArray = 0;
  _triangleArray = 0;

  _pointSize = 0;
  _lineSize = 0;
  _triangleSize = 0;

  glGenBuffers( 1, &_vertexArray ) GLE;
  if( type == XGeometry::Dynamic )
    {
    _type = GL_DYNAMIC_DRAW;
    }
  else if( type == XGeometry::Stream )
    {
    _type = GL_STREAM_DRAW;
    }
  else
    {
    _type = GL_STATIC_DRAW;
    }
  _usedCacheSize = 0;
  }

XGLGeometryCache::~XGLGeometryCache( )
    {
    glDeleteBuffers( 1, &_vertexArray ) GLE;

    if( _pointArray )
        {
        glDeleteBuffers( 1, &_pointArray ) GLE;
        }
    if( _lineArray )
        {
        glDeleteBuffers( 1, &_lineArray ) GLE;
        }
    if( _triangleArray )
        {
        glDeleteBuffers( 1, &_triangleArray ) GLE;
        }
    }

void XGLGeometryCache::setPoints( const XVector<unsigned int> &poi )
    {
    if( poi.size() )
        {
        _pointSize = poi.size();
        if( !_pointArray )
            {
            glGenBuffers( 1, &_pointArray );
            }

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _pointArray ) GLE;
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, poi.size()*sizeof(unsigned int), &(poi.front()), _type ) GLE;
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;
        }
    else if( _pointArray )
        {
        glDeleteBuffers( 1, &_pointArray ) GLE;
        }
    }

void XGLGeometryCache::setLines( const XVector<unsigned int> &lin )
    {
    if( lin.size() )
        {
        _lineSize = lin.size();
        if( !_lineArray )
            {
            glGenBuffers( 1, &_lineArray ) GLE;
            }

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _lineArray ) GLE;
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, lin.size()*sizeof(unsigned int), &(lin.front()), _type ) GLE;
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;
        }
    else if( _lineArray )
        {
        glDeleteBuffers( 1, &_lineArray ) GLE;
        }
    }

void XGLGeometryCache::setTriangles( const XVector<unsigned int> &tri )
    {
    if( tri.size() )
        {
        _triangleSize = tri.size();
        if( !_triangleArray )
            {
            glGenBuffers( 1, &_triangleArray ) GLE;
            }

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _triangleArray ) GLE;
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, tri.size()*sizeof(unsigned int), &(tri.front()), _type ) GLE;
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;
        }
    else if( _triangleArray )
        {
        glDeleteBuffers( 1, &_triangleArray );
        }
    }

void XGLGeometryCache::setAttributesSize( int s, int num1D, int num2D, int num3D, int num4D )
    {
    _usedCacheSize = 0;
    _cache.clear();
    glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
    glBufferData( GL_ARRAY_BUFFER, (sizeof(float)*s*num1D)
                                   + (sizeof(float)*2*s*num2D)
                                   + (sizeof(float)*3*s*num3D)
                                   + (sizeof(float)*4*s*num4D), 0, _type ) GLE;
    glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
    }

void XGLGeometryCache::setAttribute( QString name, const XVector<xReal> &attr )
    {
    int offset( getCacheOffset( name, 1, attr.size() ) );

    //insert data
    glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
    glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*1*sizeof(float), &attr.front() ) GLE;
    glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
    }

void XGLGeometryCache::setAttribute( QString name, const XVector<XVector2D> &attr )
    {
    int offset( getCacheOffset( name, 2, attr.size() ) );

    //insert data
    glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
    glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*2*sizeof(float), &attr.front() ) GLE;
    glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
    }

void XGLGeometryCache::setAttribute( QString name, const XVector<XVector3D> &attr )
    {
    int offset( getCacheOffset( name, 3, attr.size() ) );

    //insert data
    glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
    glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*3*sizeof(float), &attr.front() ) GLE;
    glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
    }

void XGLGeometryCache::setAttribute( QString name, const XVector<XVector4D> &attr )
    {
    int offset( getCacheOffset( name, 4, attr.size() ) );

    //insert data
    glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
    glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*4*sizeof(float), &attr.front() ) GLE;
    glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
    }

int XGLGeometryCache::getCacheOffset( const QString &name, int components, int attrSize )
    {
    foreach( const DrawCache &ref, _cache )
        {
        if( ref.name == name )
            {
            return ref.offset;
            }
        }

    DrawCache c;
    c.components = components;
    c.name = name;
    c.offset = _usedCacheSize;
    _cache << c;
    _usedCacheSize += sizeof(float) * components * attrSize;

    return c.offset;
    }
