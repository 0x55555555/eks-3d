#include "XGLRenderer.h"

#if X_ENABLE_GL_RENDERER

#include "GL/glew.h"

#include "XFramebuffer.h"
#include "XGeometry.h"
#include "XShader.h"
#include "XTexture.h"
#include "XRasteriserState.h"
#include "XBlendState.h"
#include "XDepthStencilState.h"
#include "XColour"
#include "QVarLengthArray"
#include "QDebug"
#include "XShader.h"
#include "QFile"

#define GL_REND(x) static_cast<GLRendererImpl*>(x)

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

template <typename X, typename T> void destroy(Renderer *, X *x)
  {
  x->destroy<T>();
  }

//----------------------------------------------------------------------------------------------------------------------
// RENDERER
//----------------------------------------------------------------------------------------------------------------------

class GLRendererImpl : public Renderer
  {
public:
  GLRendererImpl(const detail::RendererFunctions& fns);


  static void setTransform(Renderer *r, const Transform &trans)
    {
    GL_REND(r)->model = trans;
    GL_REND(r)->modelDataDirty = true;
    }

  static void setClearColour(Renderer *, const Colour &col)
    {
    glClearColor(col.x(), col.y(), col.z(), col.w());
    }

  static void clear(Renderer *r, FrameBuffer *fb, int c)
    {
    xAssert(GL_REND(r)->_currentFramebuffer == fb->data<XGLFramebuffer>());
    int realMode = ((c&FrameBuffer::ClearColour) != false) ? GL_COLOR_BUFFER_BIT : 0;
    realMode |= ((c&FrameBuffer::ClearDepth) != false) ? GL_DEPTH_BUFFER_BIT : 0;
    glClear( realMode ) GLE;
    }


  static void setViewTransform(Renderer *r, const Eks::Transform &trans)
    {
    GL_REND(r)->view = trans;
    GL_REND(r)->viewDataDirty = true;
    }

  static void setProjectionTransform(Renderer *r, const Eks::ComplexTransform &trans)
    {
    GL_REND(r)->projection = trans;
    GL_REND(r)->viewDataDirty = true;
    }

  static void drawIndexedTriangles(Renderer *ren, const IndexGeometry *indices, const Geometry *vert);
  static void drawTriangles(Renderer *r, const Geometry *vert);
  static void debugRenderLocator(Renderer *r, RendererDebugLocatorMode);

  Eks::Transform model;
  bool modelDataDirty;

  Eks::Transform view;
  Eks::ComplexTransform projection;
  bool viewDataDirty;

  QGLContext *_context;
  Shader *_currentShader;
  ShaderVertexLayout *_vertexLayout;
  QSize _size;
  XGLFramebuffer *_currentFramebuffer;
  };

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------
class XGLShaderResource
  {
  };

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------
class XGLTexture2D : public XGLShaderResource
  {
public:
  bool init(GLRendererImpl *, int format, int width, int height, const void *data);

  ~XGLTexture2D();

  static bool create(
      Renderer *r,
      Texture2D *tex,
      xsize w,
      xsize h,
      xuint32 format,
      const void *data)
    {
    XGLTexture2D* t = tex->create<XGLTexture2D>();
    return t->init(GL_REND(r), format, w, h, data);
    }

  static void getInfo(const Renderer *r, const Texture2D *tex, Eks::VectorUI2D& v);

private:
  void clear();
  int getInternalFormat( int format );

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
  bool init(Renderer *, TextureFormat colourFormat, TextureFormat depthFormat, xuint32 width, xuint32 height);
  bool init(GLRendererImpl *);
  ~XGLFramebuffer( );

  void bind();
  void unbind();

  virtual bool isValid() const;

  const Texture2D *colour() const;
  const Texture2D *depth() const;

  static bool create(
      Renderer *r,
      FrameBuffer *b,
      xuint32 w,
      xuint32 h,
      xuint32 colourFormat,
      xuint32 depthFormat)
    {
    XGLFramebuffer* fb = b->create<XGLFramebuffer>();
    return fb->init(GL_REND(r), (TextureFormat)colourFormat, (TextureFormat)depthFormat, w, h );
    }

  static void beginRender(Renderer *ren, FrameBuffer *fb)
    {
    GLRendererImpl *r = GL_REND(ren);
    xAssert(!r->_currentFramebuffer);

    xAssert(fb);
    r->_currentFramebuffer = fb->data<XGLFramebuffer>();

    r->_currentFramebuffer->bind();
    }

  static void endRender(Renderer *ren, FrameBuffer *fb)
    {
    GLRendererImpl *r = GL_REND(ren);

    XGLFramebuffer* iFb = fb->data<XGLFramebuffer>();;
    xAssert(r->_currentFramebuffer == iFb);

    if(r->_currentFramebuffer)
      {
      r->_currentFramebuffer->unbind();
      }

    r->_currentFramebuffer->unbind();
    r->_currentFramebuffer = 0;
    }

  static void clear(Renderer *r, FrameBuffer *buffer, xuint32 mode)
    {
#if X_ASSERTS_ENABLED
    GLRendererImpl *rend = GL_REND(r);
    XGLFramebuffer *fb = buffer->data<XGLFramebuffer>();
    xAssert(rend->_currentFramebuffer == fb);
  #endif

    xuint32 mask = ((mode&FrameBuffer::ClearColour) != 0 ? GL_COLOR_BUFFER_BIT : 0) |
                   ((mode&FrameBuffer::ClearDepth) != 0 ? GL_DEPTH_BUFFER_BIT : 0);

    glClear(mask);
    }

  static bool resize(Renderer *, ScreenFrameBuffer *, xuint32, xuint32 w, xuint32 h)
    {
    // nothing to do?
    glViewport(0,0,w,h);
    return true;
    }

  static void present(Renderer *, ScreenFrameBuffer *, bool *)
    {
    // swap buffers?
    }

  static Texture2D *getTexture(Renderer *, FrameBuffer *buffer, xuint32 mode)
    {
    xAssert(mode < FrameBuffer::TextureIdCount);
    XGLFramebuffer *fb = buffer->data<XGLFramebuffer>();
    return fb->_textures + mode;
    }

private:
  Texture2D _textures[FrameBuffer::TextureIdCount];
  unsigned int _buffer;

  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// BUFFER
//----------------------------------------------------------------------------------------------------------------------
class XGLBuffer
  {
public:
  bool init( GLRendererImpl *, const void *data, xuint32 type, xuint32 size);
  ~XGLBuffer( );

  unsigned int _buffer;
  };

//----------------------------------------------------------------------------------------------------------------------
// INDEX GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------
class XGLIndexGeometryCache : public XGLBuffer
  {
public:
  bool init(GLRendererImpl *, const void *data, IndexGeometry::Type type, xsize elementCount);

  static bool create(
      Renderer *ren,
      IndexGeometry *g,
      int elementType,
      const void *data,
      xsize elementCount)
    {
    XGLIndexGeometryCache *cache = g->create<XGLIndexGeometryCache>();
    cache->init(GL_REND(ren), data, (IndexGeometry::Type)elementType, elementCount);
    return true;
    }

  unsigned int _indexCount;
  unsigned int _indexType;
  };

//----------------------------------------------------------------------------------------------------------------------
// GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------
class XGLGeometryCache : public XGLBuffer
  {
public:
  bool init( GLRendererImpl *, const void *data, xsize elementSize, xsize elementCount );

  static bool create(
      Renderer *ren,
      Geometry *g,
      const void *data,
      xsize elementSize,
      xsize elementCount)
    {
    XGLGeometryCache *cache = g->create<XGLGeometryCache>();
    cache->init(GL_REND(ren), data, elementSize, elementCount);
    return true;
    }

  xsize _elementCount;
  };

//----------------------------------------------------------------------------------------------------------------------
// VERTEX LAYOUT
//----------------------------------------------------------------------------------------------------------------------

class XGLVertexLayout
  {
public:
  bool init(GLRendererImpl *, XGLShader* shader, const ShaderVertexLayoutDescription *, xsize count)
    {

    int location( shader->shader.attributeLocation(ref.name) );
    if( location >= 0 )
      {
      }
    }
  ~XGLVertexLayout();

  struct Attribute
    {
    xuint32 name;
    xsize stride;
    xsize offset;
    xuint8 components;
    // type is currently always float.
    };

  Eks::Vector<Attribute, 3> _attrs;

  void bind() const
    {
    xForeach(const auto &ref, layout->attributes)
      {
      glEnableVertexAttribArray(location) GLE;
      glVertexAttribPointer(
        ref.location,
        ref.components,
        GL_FLOAT,
        GL_FALSE,
        ref.stride,
        (GLvoid*)ref.offset ) GLE;
      }
    }

  void unbind() const
    {
    xForeach(const auto &ref, layout->attributes)
      {
      glDisableVertexAttribArray(ref.location) GLE;
      }
    }

  /*
   *
const XGLGeometryCache *geo

*/

  };

//----------------------------------------------------------------------------------------------------------------------
// SHADER
//----------------------------------------------------------------------------------------------------------------------

class XGLShaderComponent
  {
public:
  bool init( GLRendererImpl *, const char *data, xsize size);

  static bool createFragment(
      Renderer *r,
      ShaderFragmentComponent *f,
      const char *s,
      xsize l)
    {
    XGLShaderComponent *glS = f->create<XGLShaderComponent>();
    return glS->init(GL_REND(r), s, l);
    }

  static bool createVertex(
      Renderer *r,
      ShaderVertexComponent *v,
      const char *s,
      xsize l,
      const ShaderVertexLayoutDescription *vertexDescriptions,
      xsize vertexItemCount,
      ShaderVertexLayout *layout)
    {
    XGLShaderComponent *glS = v->create<XGLShaderComponent>();
    if(!glS->init(GL_REND(r), s, l))
      {
      return false;
      }

    if(layout)
      {
      xAssert(vertexItemCount > 0);
      xAssert(vertexDescriptions);

      XGLVertexLayout *glL = layout->create<XGLVertexLayout>();
      return glL->init(GL_REND(r), vertexDescriptions, vertexItemCount);
      }

    return true;
    }

  xuint32 component;
  };

class XGLShader
  {
public:
  bool init( GLRendererImpl *, XGLShaderComponent *, XGLShaderComponent * );
  ~XGLShader();

  bool build(QStringList &log);
  bool isValid();

  static void destroy(Renderer *r, Shader *x)
    {
    if(x == GL_REND(r)->_currentShader)
      {
      glUseProgram(0);
      GL_REND(r)->_currentShader = 0;
      }

    Eks::destroy<Shader, XGLShader>(r, x);
    }

  static bool create(
      Renderer *r,
      Shader *s,
      ShaderVertexComponent *v,
      ShaderFragmentComponent *f)
    {
    XGLShader *glS = s->create<XGLShader>();
    return glS->init(GL_REND(r), v->data<XGLShaderComponent>(), f->data<XGLShaderComponent>() );
    }

  static void bind(Renderer *ren, const Shader *shader, const ShaderVertexLayout *layout)
    {
    GLRendererImpl* r = GL_REND(ren);
    if(r->_vertexLayout)
      {
      XGLVertexLayout* oldLayout = r->_vertexLayout;
      oldLayout->unbind();
      }
    if( shader &&
        ( r->_currentShader == 0 || r->_currentShader != shader || r->_vertexLayout != layout) )
      {
      r->_currentShader = const_cast<Shader *>(shader);
      r->_vertexLayout = const_cast<ShaderVertexLayout *>(layout);
      //XGLShader* shaderInt = r->_currentShader->data<XGLShader>();

      XGLVertexLayout* newLayout = layout;
      newLayout->bind();

      xAssertFail();
      /*
      xAssert(shaderInt->shader.isLinked());

      shaderInt->shader.bind() GLE;

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
      glUseProgram(0);
      r->_currentShader = 0;
      r->_vertexLayout = 0;
      }
    }

  static void setConstantBuffers(
    Renderer *r,
    Shader *shader,
    xsize index,
    xsize count,
    const ShaderConstantData * const* data);

  static void setResources(
    Renderer *r,
    Shader *shader,
    xsize index,
    xsize count,
    const Resource * const* data);

  xuint32 shader;
  friend class XGLRenderer;
  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// SHADER DATA
//----------------------------------------------------------------------------------------------------------------------

class XGLShaderData
  {
public:
  bool init(GLRendererImpl *, xsize size, void *data);
  ~XGLShaderData();

  static void update(Renderer *r, ShaderConstantData *, void *data);

  static bool create(
      Renderer *r,
      ShaderConstantData *d,
      xsize size,
      void *data)
    {
    XGLShaderData *glD = d->create<XGLShaderData>();
    return glD->init(GL_REND(r), size, data);
    }

  friend class XGLRenderer;
  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// BLEND STATE
//----------------------------------------------------------------------------------------------------------------------

class XGLBlendState
  {
public:
  bool init(GLRendererImpl *);

  static void bind(Renderer *, const BlendState *)
    {
    xAssertFail();
    }

  static bool create(
      Renderer *r,
      BlendState *s)
    {
    (void)r;
    (void)s;
    return false;
    }
  };

//----------------------------------------------------------------------------------------------------------------------
// DEPTH STENCIL STATE
//----------------------------------------------------------------------------------------------------------------------

class XGLDepthStencilState
  {
public:
  bool init(GLRendererImpl *);

  static void bind(Renderer *, const DepthStencilState *)
    {
    xAssertFail();
    }

  static bool create(
      Renderer *r,
      DepthStencilState *s)
    {
    (void)r;
    (void)s;
    return false;
    }
  };

//----------------------------------------------------------------------------------------------------------------------
// RASTERISER STATE
//----------------------------------------------------------------------------------------------------------------------

class XGLRasteriserState
  {
public:
  bool init(GLRendererImpl *, RasteriserState::CullMode mode)
    {
    _cull = mode;
    return true;
    }

  static void bind(Renderer *, const RasteriserState *state)
    {
    const XGLRasteriserState* s = state->data<XGLRasteriserState>();
    switch(s->_cull)
      {
    case RasteriserState::CullNone:
      glDisable(GL_CULL_FACE);
      break;
    case RasteriserState::CullFront:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      break;
    case RasteriserState::CullBack:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      break;
      }
    }


  static bool create(
      Renderer *r,
      RasteriserState *s,
      xuint32 cull)
    {
    XGLRasteriserState *glS = s->create<XGLRasteriserState>();
    return glS->init(GL_REND(r), (RasteriserState::CullMode)cull);
    }

  RasteriserState::CullMode _cull;
  };

GLRendererImpl::GLRendererImpl(const detail::RendererFunctions &fns) : _context(0), _currentShader(0), _currentFramebuffer(0)
  {
  setFunctions(fns);
  }


void GLRendererImpl::debugRenderLocator(Renderer *r, RendererDebugLocatorMode m)
  {
  if((m&RendererDebugLocatorMode::DebugLocatorClearShader) != 0)
    {
    GL_REND(r)->_currentShader = 0;
    glUseProgram(0);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }

  float lineData[] =
  {
    -0.5, 0, 0,
    0.5, 0, 0,
    0, -0.5, 0,
    0, 0.5, 0,
    0, 0, -0.5,
    0, 0, 0.5
  };

  glEnableClientState(GL_VERTEX_ARRAY) GLE;
  glVertexPointer(2, GL_FLOAT, sizeof(float) * 3, lineData) GLE;
  glDrawArrays(GL_LINES, 0, 6) GLE;
  glDisableClientState(GL_VERTEX_ARRAY);

  float triData[] =
  {
    -0.5, 0, 0,
    0, 0, 0,
    0, -0.5, 0,
    0, 0, 0,
    -0.5, 0, 0,
    0, -0.5, 0,
  };

  glEnableClientState(GL_VERTEX_ARRAY) GLE;
  glVertexPointer(2, GL_FLOAT, sizeof(float) * 3, triData) GLE;
  glDrawArrays(GL_TRIANGLES, 0, 6) GLE;
  glDisableClientState(GL_VERTEX_ARRAY);
  }

/*
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

void setViewportSize(Renderer *, QSize size)
  {
  glViewport( 0, 0, size.width(), size.height() ) GLE;
  }*/

void GLRendererImpl::drawIndexedTriangles(Renderer *ren, const IndexGeometry *indices, const Geometry *vert)
  {
  GLRendererImpl* r = GL_REND(ren);
  xAssert(r->_currentShader);
  xAssert(r->_vertexLayout);
  xAssert(indices);
  xAssert(vert);

  const XGLIndexGeometryCache *idx = indices->data<XGLIndexGeometryCache>();
  const XGLGeometryCache *gC = vert->data<XGLGeometryCache>();
  const XGLVertexLayout *layout = r->_vertexLayout->data<XGLVertexLayout>();


  layout->bind(gC);

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, idx->_buffer ) GLE;

  glDrawElements( GL_TRIANGLES, idx->_indexCount, idx->_indexType, (GLvoid*)((char*)NULL)) GLE;

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) GLE;

  layout->unbind();
  }

void GLRendererImpl::drawTriangles(Renderer *ren, const Geometry *vert)
  {
  GLRendererImpl* r = GL_REND(ren);
  xAssert(r->_currentShader);
  xAssert(r->_vertexLayout);
  xAssert(vert);

  const XGLGeometryCache *gC = vert->data<XGLGeometryCache>();
  const XGLVertexLayout *layout = r->_vertexLayout->data<XGLVertexLayout>();

  glBindBuffer( GL_ARRAY_BUFFER, idx->_buffer ) GLE;

  glDrawArrays( GL_TRIANGLES, 0, gC->_elementCount) GLE;

  glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
  }

detail::RendererFunctions glfns =
  {
    {
      XGLFramebuffer::create,
      XGLGeometryCache::create,
      XGLIndexGeometryCache::create,
      XGLTexture2D::create,
      XGLShader::create,
      XGLShaderComponent::createVertex,
      XGLShaderComponent::createFragment,
      XGLRasteriserState::create,
      XGLDepthStencilState::create,
      XGLBlendState::create,
      XGLShaderData::create
    },
    {
      destroy<FrameBuffer, XGLFramebuffer>,
      destroy<Geometry, XGLGeometryCache>,
      destroy<IndexGeometry, XGLIndexGeometryCache>,
      destroy<Texture2D, XGLTexture2D>,
      XGLShader::destroy,
      destroy<ShaderVertexLayout, XGLVertexLayout>,
      destroy<ShaderVertexComponent, XGLShaderComponent>,
      destroy<ShaderFragmentComponent, XGLShaderComponent>,
      destroy<RasteriserState, XGLRasteriserState>,
      destroy<DepthStencilState, XGLDepthStencilState>,
      destroy<BlendState, XGLBlendState>,
      destroy<ShaderConstantData, XGLShaderData>
    },
    {
      GLRendererImpl::setClearColour,
      XGLShaderData::update,
      GLRendererImpl::setViewTransform,
      GLRendererImpl::setProjectionTransform,
      XGLShader::setConstantBuffers,
      XGLShader::setResources,
      XGLShader::bind,
      XGLRasteriserState::bind,
      XGLDepthStencilState::bind,
      XGLBlendState::bind,
      GLRendererImpl::setTransform
    },
    {
      XGLTexture2D::getInfo
    },
    {
      GLRendererImpl::drawIndexedTriangles,
      GLRendererImpl::drawTriangles,
      GLRendererImpl::debugRenderLocator
    },
    {
      XGLFramebuffer::clear,
      XGLFramebuffer::resize,
      XGLFramebuffer::beginRender,
      XGLFramebuffer::endRender,
      XGLFramebuffer::present,
      XGLFramebuffer::getTexture
    }
  };

Renderer *GLRenderer::createGLRenderer(ScreenFrameBuffer *buffer, Eks::AllocatorBase* alloc)
  {
  GLRendererImpl *r = alloc->create<GLRendererImpl>(glfns);
  glEnable( GL_DEPTH_TEST ) GLE;

  XGLFramebuffer* fb = buffer->create<XGLFramebuffer>();
  fb->init(r);

  return r;
  }

void GLRenderer::destroyGLRenderer(Renderer *r, ScreenFrameBuffer *buffer, Eks::AllocatorBase* alloc)
  {
  buffer->destroy<XGLFramebuffer>();

  alloc->destroy(GL_REND(r));
  }

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------
bool XGLTexture2D::init(GLRendererImpl *, int format, int width, int height, const void *data)
  {
  glGenTextures(1, &_id) GLE;
  glBindTexture(GL_TEXTURE_2D, _id) GLE;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) GLE;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) GLE;

  // could also be GL_REPEAT
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) GLE;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) GLE;

  int formatMap[] =
  {
    GL_RGBA,
    GL_DEPTH_COMPONENT24
  };
  xCompileTimeAssert(X_ARRAY_COUNT(formatMap) == TextureFormatCount);

  // 0 at end could be data to unsigned byte...
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, formatMap[format], GL_UNSIGNED_BYTE, data) GLE;

  glBindTexture(GL_TEXTURE_2D, 0) GLE;

  return true;
  }

XGLTexture2D::~XGLTexture2D()
  {
  clear();
  }


void XGLTexture2D::clear()
  {
  glDeleteTextures(1, &_id) GLE;
  }

//----------------------------------------------------------------------------------------------------------------------
// FRAMEBUFFER
//----------------------------------------------------------------------------------------------------------------------
bool XGLFramebuffer::init(Renderer *r, TextureFormat cF, TextureFormat dF, xuint32 width, xuint32 height)
  {
  glGenFramebuffers(1, &_buffer) GLE;
  glBindFramebuffer(GL_FRAMEBUFFER, _buffer) GLE;

  if(!Texture2D::delayedCreate(_textures[FrameBuffer::TextureColour], r, width, height, cF, 0))
    {
    return false;
    }
  XGLTexture2D* c = _textures[FrameBuffer::TextureColour].data<XGLTexture2D>();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, c->_id, 0) GLE;

  if(!Texture2D::delayedCreate(_textures[FrameBuffer::TextureDepthStencil], r, width, height, dF, 0))
    {
    return false;
    }
  XGLTexture2D* d = _textures[FrameBuffer::TextureDepthStencil].data<XGLTexture2D>();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d->_id, 0) GLE;

  glBindFramebuffer(GL_FRAMEBUFFER, 0) GLE;
  return isValid();
  }

bool XGLFramebuffer::init(GLRendererImpl *)
  {
  _buffer = 0;
  return true;
  }

XGLFramebuffer::~XGLFramebuffer( )
  {
  if( _buffer )
    {
    glDeleteFramebuffers( 1, &_buffer ) GLE;
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

//----------------------------------------------------------------------------------------------------------------------
// SHADER
//----------------------------------------------------------------------------------------------------------------------
XGLShader::~XGLShader()
  {
  glDeleteProgram(shader);
  }

//bool XGLShader::build(QStringList &log)
//  {
//  bool result = shader.link() GLE;

//  QString logEntry = shader.log();
//  if(!logEntry.isEmpty())
//    {
//    log << logEntry;
//    }

//  return result;
//  }

//bool XGLShader::isValid()
//  {
//  bool result = shader.isLinked() GLE;
//  return result;
//  }

//XAbstractShaderVariable *XGLShader::createVariable( QString in, XAbstractShader *s )
//  {
//  XGLShaderVariable* var = new XGLShaderVariable( s, in );
//  return var;
//  }

//void XGLShader::destroyVariable( XAbstractShaderVariable *var )
//  {
//  delete var;
//  }

//----------------------------------------------------------------------------------------------------------------------
// BUFFER
//----------------------------------------------------------------------------------------------------------------------
bool XGLBuffer::init( GLRendererImpl *, const void *data, xuint32 type, xuint32 size)
  {
  glGenBuffers(1, &_buffer);

  glBindBuffer(type, _buffer) GLE;
  glBufferData(type, size, data, GL_STATIC_DRAW) GLE;
  glBindBuffer(type, 0) GLE;

  return true;
  }


XGLBuffer::~XGLBuffer( )
  {
  glDeleteBuffers(1, &_buffer) GLE;
  }

//----------------------------------------------------------------------------------------------------------------------
// INDEX GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------
bool XGLIndexGeometryCache::init(GLRendererImpl *r, const void *data, IndexGeometry::Type type, xsize elementCount)
  {
  struct Type
    {
    xuint32 type;
    xuint32 size;
    };

  Type typeMap[] =
    {
    { GL_UNSIGNED_SHORT, sizeof(xuint16) }
    };
  xCompileTimeAssert(IndexGeometry::TypeCount == X_ARRAY_COUNT(typeMap));

  _indexType = typeMap[type].type;
  _indexCount = elementCount;

  xsize dataSize = elementCount * typeMap[type].size;
  return XGLBuffer::init(r, data, GL_ELEMENT_ARRAY_BUFFER, dataSize);
  }

//----------------------------------------------------------------------------------------------------------------------
// GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------

bool XGLGeometryCache::init(GLRendererImpl *r, const void *data, xsize elementSize, xsize elementCount)
  {
  xsize dataSize = elementSize * elementCount;
  _elementCount = elementCount;
  return XGLBuffer::init(r, data, GL_ARRAY_BUFFER, dataSize);
  }

}

#endif
