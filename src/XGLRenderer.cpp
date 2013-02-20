#include "XGLRenderer.h"

#if X_ENABLE_GL_RENDERER

#include "XFramebuffer.h"
#include "XGeometry.h"
#include "XShader.h"
#include "XTexture.h"
#include "XRasteriserState.h"
#include "XColour"
#include "QGLShaderProgram"
#include "QVarLengthArray"
#include "QDebug"
#include "XShader.h"
#include "QFile"

#define GL_R(x) static_cast<GLESRendererImpl*>(x)

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
  else if( err == GL_OUT_OF_MEMORY )
    {
    return "GL Error: Out Of Memory";
    }
  else if ( err )
    {
    return "GL Error: Unknown";
    }
  return "GL Error: No Error";
  }

#ifdef X_DEBUG
# define GLE ; { int _GL = glGetError(); xAssertMessage(!_GL, "GL Error", _GL, glErrorString( _GL )); }
# define GLE_QUIET ; glGetError()
#else
# define GLE
# define GLE_QUIET
//# define GLE ; { int _GL = glGetError(); if( _GL ) { qCritical() << __FILE__ << __LINE__<< glErrorString( _GL ); } }
//# define GLE_QUIET ; glGetError()
#endif

namespace Eks
{

class GLESRendererImpl;

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------

class XGLTexture2D
  {
public:
  bool init(GLESRendererImpl *, int format, int width, int height, const void *data);

  ~XGLTexture2D();

private:
  void clear();

  unsigned int _id;

  friend class XGLFramebuffer;
  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// FRAMEBUFFER
//----------------------------------------------------------------------------------------------------------------------

class XGLFramebuffer
  {
public:
  bool init(GLESRendererImpl *, int colourFormat, int depthFormat, int width, int height);
  bool init(GLESRendererImpl *);
  ~XGLFramebuffer( );

  void bind();
  void unbind();

  virtual bool isValid() const;

  const Texture2D *colour() const;
  const Texture2D *depth() const;

private:
  XGLTexture2D *_colour;
  XGLTexture2D *_depth;
  unsigned int _buffer;

  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// INDEX GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------

class XGLIndexGeometryCache
  {
public:
  bool init( GLESRendererImpl *, const void *data, IndexGeometry::Type type, xsize elementCount );
  ~XGLIndexGeometryCache( );

  unsigned int _indexArray;
  unsigned int _indexCount;
  unsigned int _indexType;
  };

//----------------------------------------------------------------------------------------------------------------------
// GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------


class XGLGeometryCache
  {
public:
  bool init( GLESRendererImpl *, const void *data, xsize elementSize, xsize elementCount );
  ~XGLGeometryCache( );

  unsigned int _vertexArray;
  };

//----------------------------------------------------------------------------------------------------------------------
// VERTEX LAYOUT
//----------------------------------------------------------------------------------------------------------------------

class XGLVertexLayout
  {
public:
  bool init(GLESRendererImpl *, const ShaderVertexLayoutDescription *, xsize count);
  ~XGLVertexLayout();

  struct Attribute
    {
    QString name;
    xsize stride;
    xsize offset;
    xuint8 components;
    // type is currently always float.
    };

  enum
    {
    AttributeCount = 3
    };
  Eks::Vector<Attribute, AttributeCount> attributes;

  void bind(const XGLGeometryCache *geo) const;
  void unbind() const;

  /*

  glBindBuffer( GL_ARRAY_BUFFER, gC->_vertexArray ) GLE;
  Q_FOREACH( const XGLVertexLayout::Attribute &ref, layout->attributes )
    {
    int location( shader->shader.attributeLocation(ref.name) );
    if( location >= 0 )
      {
      m_ids << location;
      glEnableVertexAttribArray( location ) GLE;
      glVertexAttribPointer( location, ref.components, GL_FLOAT, GL_FALSE, ref.stride, (GLvoid*)ref.offset ) GLE;
      }
    }
  Q_FOREACH( int id, m_ids )
    {
    glDisableVertexAttribArray( id ) GLE;
    }

  glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
*/

  };

//----------------------------------------------------------------------------------------------------------------------
// SHADER
//----------------------------------------------------------------------------------------------------------------------

class XGLShaderComponent
  {
public:
  bool init( GLESRendererImpl *, const char *data, xsize size);

  QGLShader component;
  };

class XGLShader
  {
public:
  bool init( GLESRendererImpl *, XGLShaderComponent *, XGLShaderComponent * );
  ~XGLShader();

  bool build(QStringList &log);
  bool isValid();


  QGLShaderProgram shader;
  friend class XGLRenderer;
  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// SHADER DATA
//----------------------------------------------------------------------------------------------------------------------

class XGLShaderData
  {
public:
  bool init(GLESRendererImpl *, xsize size, void *data);
  ~XGLShaderData();

  void update(void *data);

  friend class XGLRenderer;
  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// RASTERISER STATE
//----------------------------------------------------------------------------------------------------------------------

class XGLRasteriserState
  {

  };

//----------------------------------------------------------------------------------------------------------------------
// RENDERER
//----------------------------------------------------------------------------------------------------------------------

class GLESRendererImpl : public Renderer
  {
public:
  GLESRendererImpl(const detail::RendererFunctions& fns);


  Eks::Transform model;
  bool modelDataDirty;

  Eks::Transform view;
  Eks::Transform projection;
  bool viewDataDirty;

  QGLContext *_context;
  Shader *_currentShader;
  ShaderVertexLayout *_vertexLayout;
  QSize _size;
  XGLFramebuffer *_currentFramebuffer;
  };

GLESRendererImpl::GLESRendererImpl(const detail::RendererFunctions &fns) : _context(0), _currentShader(0), _currentFramebuffer(0)
  {
  setFunctions(fns);
  }

void setTransform(Renderer *r, const Transform &trans)
  {
  GL_R(r)->model = trans;
  GL_R(r)->modelDataDirty = true;
  }

void setClearColour(Renderer *, const Colour &col)
  {
  glClearColor(col.x(), col.y(), col.z(), col.w());
  }

void clear(Renderer *r, FrameBuffer *fb, int c)
  {
  xAssert(GL_R(r)->_currentFramebuffer == fb->data<XGLFramebuffer>());
  int realMode = ((c&FrameBuffer::ClearColour) != false) ? GL_COLOR_BUFFER_BIT : 0;
  realMode |= ((c&FrameBuffer::ClearDepth) != false) ? GL_DEPTH_BUFFER_BIT : 0;
  glClear( realMode ) GLE;
  }

/*
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


  glBegin(GL_TRIANGLES);
  glVertex3f(-0.5, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, -0.5, 0);

  glVertex3f(0, 0, 0);
  glVertex3f(-0.5, 0, 0);
  glVertex3f(0, -0.5, 0);
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
  }*/

void setViewportSize(Renderer *, QSize size)
  {
  glViewport( 0, 0, size.width(), size.height() ) GLE;
  }

void setViewTransform(Renderer *r, const Eks::Transform &trans)
  {
  GL_R(r)->view = trans;
  GL_R(r)->viewDataDirty = true;
  }

void setProjectionTransform(Renderer *r, const Eks::ComplexTransform &trans)
  {
  GL_R(r)->projection = trans;
  GL_R(r)->viewDataDirty = true;
  }

void drawTriangles(Renderer *ren, const IndexGeometry *indices, const Geometry *vert)
  {
  GLESRendererImpl* r = GL_R(ren);
  xAssert(r->_currentShader);
  xAssert(r->_vertexLayout);
  xAssert(indices);
  xAssert(vert);

  const XGLIndexGeometryCache *idx = indices->data<XGLIndexGeometryCache>();
  const XGLGeometryCache *gC = vert->data<XGLGeometryCache>();
  const XGLVertexLayout *layout = r->_vertexLayout->data<XGLVertexLayout>();


  layout->bind(gC);

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, idx->_indexArray ) GLE;

  glDrawElements( GL_TRIANGLES, idx->_indexCount, idx->_indexType, (GLvoid*)((char*)NULL)) GLE;

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;

  layout->unbind();
  }

bool createGeometry(
    Renderer *ren,
    Geometry *g,
    const void *data,
    xsize elementSize,
    xsize elementCount)
  {
  XGLGeometryCache *cache = g->create<XGLGeometryCache>();
  cache->init(GL_R(ren), data, elementSize, elementCount);
  return true;
  }

bool createIndexGeometry(
    Renderer *ren,
    IndexGeometry *g,
    int elementType,
    const void *data,
    xsize elementCount)
  {
  XGLIndexGeometryCache *cache = g->create<XGLIndexGeometryCache>();
  cache->init(GL_R(ren), data, (IndexGeometry::Type)elementType, elementCount);
  return true;
  }

void setShader(Renderer *ren, const Shader *shader, const ShaderVertexLayout *layout)
  {
  GLESRendererImpl* r = GL_R(ren);
  if( shader &&
      ( r->_currentShader == 0 || r->_currentShader != shader || r->_vertexLayout != layout) )
    {
    r->_currentShader = const_cast<Shader *>(shader);
    r->_vertexLayout = const_cast<ShaderVertexLayout *>(layout);
    XGLShader* shaderInt = r->_currentShader->data<XGLShader>();

    xAssert(shaderInt->shader.isLinked());

    shaderInt->shader.bind() GLE;

    xAssertFail();
    /*
    int x=0;
    Q_FOREACH( ShaderVariable *var, shader->variables() )
      {
      XGLShaderVariable *glVar( static_cast<XGLShaderVariable*>(var->internal()) );
      if( glVar->_texture )
        {
        const XTexture *tex( glVar->_texture );
        tex->prepareInternal( this );
        const XGLTexture2D *glTex( static_cast<const XGLTexture2D*>(tex->internal()) );
        xAssert( glTex );
        glActiveTexture( GL_TEXTURE0 + x ) GLE;
        glBindTexture( GL_TEXTURE_2D, glTex->_id ) GLE;
        shaderInt->shader.setUniformValue( glVar->_location, x );
        }
      x++;
      }*/
    }
  else if( shader == 0 && r->_currentShader != 0 )
    {
    XGLShader* shaderInt = r->_currentShader->data<XGLShader>();
    shaderInt->shader.release() GLE;
    r->_currentShader = 0;
    r->_vertexLayout = 0;
    }
  }

bool createFragmentShaderComponent(
    Renderer *r,
    ShaderFragmentComponent *f,
    const char *s,
    xsize l)
  {
  XGLShaderComponent *glS = f->create<XGLShaderComponent>();
  glS->init(GL_R(r), s, l);
  }

bool createVertexShaderComponent(
    Renderer *r,
    ShaderVertexComponent *v,
    const char *s,
    xsize l,
    const ShaderVertexLayoutDescription *vertexDescriptions,
    xsize vertexItemCount,
    ShaderVertexLayout *layout)
  {
  XGLShaderComponent *glS = v->create<XGLShaderComponent>();
  glS->init(GL_R(r), s, l);

  if(layout)
    {
    xAssert(vertexItemCount > 0);
    xAssert(vertexDescriptions);

    XGLVertexLayout *glL = layout->create<XGLVertexLayout>();
    glL->init(GL_R(r), vertexDescriptions, vertexItemCount);
    }
  }

bool createShader(
    Renderer *r,
    Shader *s,
    ShaderVertexComponent *v,
    ShaderFragmentComponent *f)
  {
  XGLShader *glS = s->create<XGLShader>();
  glS->init(GL_R(r), v->data<XGLShaderComponent>(), f->data<XGLShaderComponent>() );
  }

bool createShaderConstantData(
    Renderer *r,
    ShaderConstantData *d,
    xsize size,
    void *data)
  {
  XGLShaderData *glD = d->create<XGLShaderData>();
  glD->init(GL_R(r), size, data);
  }

bool createRasteriserState(
    Renderer *r,
    RasteriserState *s,
    xuint32 cull)
  {
  XGLRasteriserState *glS = f->create<XGLRasteriserState>();
  glS->init(GL_R(r), (RasteriserState::CullMode)cull);
  }

void beginRender(Renderer *ren, FrameBuffer *fb)
  {
  GLESRendererImpl *r = GL_R(ren);
  xAssert(!r->_currentFramebuffer);

  xAssert(fb);
  r->_currentFramebuffer = fb->data<XGLFramebuffer>();

  r->_currentFramebuffer->bind();
  }

void endRender(Renderer *ren, FrameBuffer *fb)
  {
  GLESRendererImpl *r = GL_R(ren);

  XGLFramebuffer* iFb = fb->data<XGLFramebuffer>();;
  xAssert(r->_currentFramebuffer == iFb);

  if(r->_currentFramebuffer)
    {
    r->_currentFramebuffer->unbind();
    }

  r->_currentFramebuffer->unbind();
  r->_currentFramebuffer = 0;
  }

bool createFramebuffer(
    Renderer *r,
    FrameBuffer *b,
    xuint32 w,
    xuint32 h,
    xuint32 colourFormat,
    xuint32 depthFormat)
  {
  XGLFramebuffer* fb = b->create<XGLFramebuffer>();
  return fb->init(GL_R(r), colourFormat, depthFormat, w, h );
  }

bool createTexture2D(
    Renderer *r,
    Texture2D *tex,
    xsize w,
    xsize h,
    xuint32 format,
    const void *data)
  {
  XGLTexture2D* t = tex->create<XGLTexture2D>();
  return t->init(GL_R(r), format, w, h, data);
  }

template <typename X, typename T> void destroy(Renderer *, X *x)
  {
  x->destroy<T>();
  }

detail::RendererFunctions fns =
  {
    {
      createFramebuffer,
      createGeometry,
      createIndexGeometry,
      createTexture2D,
      createShader,
      createVertexShaderComponent,
      createFragmentShaderComponent,
      createRasteriserState,
      createShaderConstantData
    },
    {
      destroy<FrameBuffer, XGLFramebuffer>,
      destroy<Geometry, XGLGeometryCache>,
      destroy<IndexGeometry, XGLIndexGeometryCache>,
      destroy<Texture2D, XGLTexture2D>,
      destroy<Shader, XGLShader>,
      destroy<ShaderVertexLayout, XGLVertexLayout>,
      destroy<ShaderVertexComponent, XGLShaderComponent>,
      destroy<ShaderFragmentComponent, XGLShaderComponent>,
      destroy<RasteriserState, XGLRasteriserState>,
      destroy<ShaderConstantData, XGLShaderData>
    },
    {
      setClearColour,
      updateShaderConstantData,
      setViewTransform,
      setProjectionTransform,
      setFragmentShaderConstantBuffer,
      setVertexShaderConstantBuffer,
      setFragmentShaderResource,
      setVertexShaderResource,
      setShader,
      setRasteriserState,
      setTransform
    },
    {
      getTexture2DInfo
    },
    {
      drawIndexedTriangles,
      drawTriangles,
      debugRenderLocator
    },
    {
      clear,
      resize,
      beginFrame,
      endFrame,
      present,
      getFramebufferTexture
    }
  };

Renderer *GLESRenderer::createGLRenderer(ScreenFrameBuffer *buffer)
  {
  GLESRendererImpl *r = new GLESRendererImpl(fns);
  glEnable( GL_DEPTH_TEST ) GLE;

  XGLFramebuffer* fb = buffer->create<XGLFramebuffer>();
  fb->init(r);

  return r;
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
  glBindTexture(GL_TEXTURE_2D, 0);
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
  XGLShaderVariable* var = new XGLShaderVariable( s, in );
  return var;
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
  clear();
  }

void XGLShaderVariable::setValue( int value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, value ) GLE;
  }

void XGLShaderVariable::setValue( Real value )
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

void XGLShaderVariable::setValue( const Colour &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, toQt(value) ) GLE;
  }

void XGLShaderVariable::setValue( const Eks::Vector2D &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, toQt(value) ) GLE;
  }

void XGLShaderVariable::setValue( const Eks::Vector3D &value )
  {
  clear();
  bindShader();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, toQt(value) ) GLE;
  }

void XGLShaderVariable::setValue( const Eks::Vector4D &value )
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

void XGLShaderVariable::setValue( const XTexture *value )
  {
  clear();
  bindShader();
  _texture = value;
  _texture->prepareInternal( abstractShader()->renderer() ) GLE; // todo, use texture units.
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValue( _location, (int)0) GLE; // static_cast<XGLTexture*>(_texture->internal())->_id) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<int> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<Real> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size(), 1 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<unsigned int> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, &(values.front()), values.size() ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<Colour> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, values.front().data(), values.size(), 4 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<Eks::Vector2D> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, values.front().data(), values.size(), 2 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<Eks::Vector3D> &values )
  {
  clear();
  GL_SHADER_VARIABLE_PARENT->shader.setUniformValueArray( _location, values.front().data(), values.size(), 3 ) GLE;
  }

void XGLShaderVariable::setValueArray( const XVector<Eks::Vector4D> &values )
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
  _texture = 0;
  }

#undef GL_SHADER_VARIABLE_PARENT

//----------------------------------------------------------------------------------------------------------------------
// INDEX GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------
XGLIndexGeometryCache::XGLIndexGeometryCache( XGLRenderer *, const void *data, IndexGeometry::Type type, xsize elementCount )
  {
  unsigned int typeMap[] =
    {
    GL_UNSIGNED_SHORT
    };
  xCompileTimeAssert(IndexGeometry::TypeCount == X_ARRAY_COUNT(typeMap));

  _indexType = typeMap[type];
  _indexCount = elementCount;
  }

//----------------------------------------------------------------------------------------------------------------------
// GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------

XGLGeometryCache::XGLGeometryCache( XGLRenderer *r, const void *data, xsize elementSize, xsize elementCount ) : _renderer( r )
  {
  _pointArray = 0;
  _lineArray = 0;
  _triangleArray = 0;

  _pointSize = 0;
  _lineSize = 0;
  _triangleSize = 0;

  glGenBuffers( 1, &_vertexArray ) GLE;
  if( type == Geometry::Dynamic )
    {
    _type = GL_DYNAMIC_DRAW;
    }
  else if( type == Geometry::Stream )
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

void XGLGeometryCache::setAttribute( QString name, const XVector<Real> &attr )
  {
  int offset( getCacheOffset( name, 1, attr.size() ) );

  //insert data
  glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
  glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*1*sizeof(float), &attr.front() ) GLE;
  glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
  }

void XGLGeometryCache::setAttribute( QString name, const XVector<Eks::Vector2D> &attr )
  {
  int offset( getCacheOffset( name, 2, attr.size() ) );

  //insert data
  glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
  glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*2*sizeof(float), &attr.front() ) GLE;
  glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
  }

void XGLGeometryCache::setAttribute( QString name, const XVector<Eks::Vector3D> &attr )
  {
  int offset( getCacheOffset( name, 3, attr.size() ) );

  //insert data
  glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
  glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*3*sizeof(float), &attr.front() ) GLE;
  glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
  }

void XGLGeometryCache::setAttribute( QString name, const XVector<Eks::Vector4D> &attr )
  {
  int offset( getCacheOffset( name, 4, attr.size() ) );

  //insert data
  glBindBuffer( GL_ARRAY_BUFFER, _vertexArray ) GLE;
  glBufferSubData( GL_ARRAY_BUFFER, offset, attr.size()*4*sizeof(float), &attr.front() ) GLE;
  glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
  }

int XGLGeometryCache::getCacheOffset( const QString &name, int components, int attrSize )
  {
  Q_FOREACH( const DrawCache &ref, _cache )
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

}

#endif
