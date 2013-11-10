#include "XGLRenderer.h"

#ifdef X_ENABLE_GL_RENDERER

#ifdef QT_OPENGL_ES_2
# define USE_GLES
#else
# define USE_GLEW
#endif

#ifdef USE_GLEW
# include "GL/glew.h"
#endif

#ifdef USE_GLES
# include "QGLFunctions"
#endif

#include "XStringSimple"
#include "XAllocatorBase"
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
class XGL21ShaderData;
class XGLVertexLayout;

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
  GLRendererImpl(const detail::RendererFunctions& fns, AllocatorBase *alloc);

  static void setTransform(Renderer *r, const Transform &trans)
    {
    GL_REND(r)->_modelData.model = trans.matrix();
    GL_REND(r)->_modelDataDirty = true;
    }

  static void setClearColour(Renderer *, const Colour &col)
    {
    glClearColor(col.x(), col.y(), col.z(), col.w());
    }

  static void setViewTransform(Renderer *r, const Eks::Transform &trans)
    {
    GL_REND(r)->_viewData.view = trans.matrix();
    GL_REND(r)->_viewDataDirty = true;
    }

  static void setProjectionTransform(Renderer *r, const Eks::ComplexTransform &trans)
    {
    GL_REND(r)->_viewData.proj = trans.matrix();
    GL_REND(r)->_viewDataDirty = true;
    }

  static void drawIndexedTriangles(Renderer *ren, const IndexGeometry *indices, const Geometry *vert);
  static void drawIndexedPrimitive(xuint32 prim, Renderer *r, const IndexGeometry *indices, const Geometry *vert);
  static void drawPrimitive(xuint32 prim, Renderer *r, const Geometry *vert);
  static void drawTriangles(Renderer *r, const Geometry *vert);
  static void drawLines(Renderer *r, const Geometry *vert);
  static void drawIndexedLines(Renderer *r, const IndexGeometry *indices, const Geometry *vert);
  static void debugRenderLocator(Renderer *r, RendererDebugLocatorMode);

  static Shader *stockShader(Renderer *r, RendererShaderType t, const ShaderVertexLayout **);
  static void setStockShader(Renderer *, RendererShaderType, Shader *, const ShaderVertexLayout *);

  enum
    {
    ConstantBufferIndexOffset = 2
    };

  Eks::AllocatorBase *_allocator;

  Eks::ShaderConstantData _model;
  Eks::ShaderConstantData _view;

  struct ModelMatrices
    {
    Eks::Matrix4x4 model;
    Eks::Matrix4x4 modelView;
    Eks::Matrix4x4 modelViewProj;
    };
  struct ViewMatrices
    {
    Eks::Matrix4x4 view;
    Eks::Matrix4x4 proj;
    };
  ModelMatrices _modelData;
  ViewMatrices _viewData;
  bool _modelDataDirty;
  bool _viewDataDirty;

  void updateViewData();

  Shader *stockShaders[ShaderTypeCount];
  const ShaderVertexLayout *stockLayouts[ShaderTypeCount];

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
public:
  void bindResource(xuint32 active) const
    {
    glActiveTexture(GL_TEXTURE0 + active);
    glBindTexture(_type, _id);
    }

  xuint32 _type;
  xuint32 _id;
  };

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------
class XGLTexture2D : public XGLShaderResource
  {
public:
  bool init(GLRendererImpl *, xuint32 format, xsize width, xsize height, const void *data);

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


  void bind() const
    {
    glBindTexture(GL_TEXTURE_2D, _id);
    }

  void unbind() const
    {
    glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
  void clear();
  int getInternalFormat( int format );

  VectorUI2D size;

  friend class XGLFramebuffer;
  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// FRAMEBUFFER
//----------------------------------------------------------------------------------------------------------------------
class XGLFramebuffer
  {
public:
  bool init(GLRendererImpl *);

  const Texture2D *colour() const;
  const Texture2D *depth() const;

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

  static bool resize(Renderer *, ScreenFrameBuffer *, xuint32 w, xuint32 h, xuint32)
    {
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

protected:
  Texture2D _textures[FrameBuffer::TextureIdCount];
  unsigned int _buffer;
  GLRendererImpl *_impl;
  };


class XGL21Framebuffer : public XGLFramebuffer
  {
public:
  bool init(Renderer *, TextureFormat colourFormat, TextureFormat depthFormat, xuint32 width, xuint32 height);
  ~XGL21Framebuffer( );

  static bool create(
      Renderer *r,
      FrameBuffer *b,
      xuint32 w,
      xuint32 h,
      xuint32 colourFormat,
      xuint32 depthFormat)
    {
    XGL21Framebuffer* fb = b->create<XGL21Framebuffer>();
    return fb->init(GL_REND(r), (TextureFormat)colourFormat, (TextureFormat)depthFormat, w, h );
    }

  static void beginRender(Renderer *ren, FrameBuffer *fb)
    {
    GLRendererImpl *r = GL_REND(ren);
    xAssert(!r->_currentFramebuffer);

    XGL21Framebuffer *fbImpl = fb->data<XGL21Framebuffer>();
    xAssert(fb);
    r->_currentFramebuffer = fbImpl;
    fbImpl->bind(r);

    clear(ren, fb, FrameBuffer::ClearColour|FrameBuffer::ClearDepth);
    }

  static void endRender(Renderer *ren, FrameBuffer *fb)
    {
    GLRendererImpl *r = GL_REND(ren);

    XGL21Framebuffer *iFb = fb->data<XGL21Framebuffer>();
    xAssert(r->_currentFramebuffer == iFb);

    if(r->_currentFramebuffer)
      {
      iFb->unbind(r);
      r->_currentFramebuffer = 0;
      }
    }

  void bind(GLRendererImpl *r);
  void unbind(GLRendererImpl *r);

  bool isValid(GLRendererImpl *impl) const;
  };


class XGL33Framebuffer : public XGLFramebuffer
  {
public:
  bool init(Renderer *, TextureFormat colourFormat, TextureFormat depthFormat, xuint32 width, xuint32 height);
  ~XGL33Framebuffer( );

  static void beginRender(Renderer *ren, FrameBuffer *fb)
    {
    GLRendererImpl *r = GL_REND(ren);
    xAssert(!r->_currentFramebuffer);

    XGL33Framebuffer *fbImpl = fb->data<XGL33Framebuffer>();
    xAssert(fb);
    r->_currentFramebuffer = fbImpl;
    fbImpl->bind(r);

    clear(ren, fb, FrameBuffer::ClearColour|FrameBuffer::ClearDepth);
    }

  static void endRender(Renderer *ren, FrameBuffer *fb)
    {
    GLRendererImpl *r = GL_REND(ren);

    XGL33Framebuffer *iFb = fb->data<XGL33Framebuffer>();
    xAssert(r->_currentFramebuffer == iFb);

    if(r->_currentFramebuffer)
      {
      iFb->unbind(r);
      r->_currentFramebuffer = 0;
      }
    }

  static bool create(
      Renderer *r,
      FrameBuffer *b,
      xuint32 w,
      xuint32 h,
      xuint32 colourFormat,
      xuint32 depthFormat)
    {
    XGL33Framebuffer* fb = b->create<XGL33Framebuffer>();
    return fb->init(GL_REND(r), (TextureFormat)colourFormat, (TextureFormat)depthFormat, w, h );
    }

  void bind(GLRendererImpl *r);
  void unbind(GLRendererImpl *r);

  bool isValid(GLRendererImpl *impl) const;
  };

//----------------------------------------------------------------------------------------------------------------------
// BUFFER
//----------------------------------------------------------------------------------------------------------------------
class XGLBuffer
  {
public:
  bool init(GLRendererImpl *, const void *data, xuint32 type, xuint32 renderType, xsize size);
  ~XGLBuffer();

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

  GLuint _indexCount;
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

  GLuint _elementCount;
  GLuint _elementSize;
  };

//----------------------------------------------------------------------------------------------------------------------
// SHADER COMPONENT
//----------------------------------------------------------------------------------------------------------------------

class XGLShaderComponent
  {
public:
  bool init(GLRendererImpl *, xuint32 type, const char *data, xsize size);

  static bool createFragment(
      Renderer *r,
      ShaderFragmentComponent *f,
      const char *s,
      xsize l)
    {
    XGLShaderComponent *glS = f->create<XGLShaderComponent>();
    glS->_layout = 0;
    return glS->init(GL_REND(r), GL_FRAGMENT_SHADER, s, l);
    }

  static bool createVertex(
      Renderer *r,
      ShaderVertexComponent *v,
      const char *s,
      xsize l,
      const ShaderVertexLayoutDescription *vertexDescriptions,
      xsize vertexItemCount,
      ShaderVertexLayout *layout);

  xuint32 _component;
  XGLVertexLayout* _layout;
  };

//----------------------------------------------------------------------------------------------------------------------
// SHADER
//----------------------------------------------------------------------------------------------------------------------
class XGLShader
  {
public:
  bool init( GLRendererImpl *impl, XGLShaderComponent *v, XGLShaderComponent *f);

  ~XGLShader();

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

  static void bind(Renderer *ren, const Shader *shader, const ShaderVertexLayout *layout);

  static void setConstantBuffers(
      Renderer *r,
      Shader *shader,
      xsize index,
      xsize count,
      const ShaderConstantData * const* data)
    {
    setConstantBuffersInternal(r, shader, index + GLRendererImpl::ConstantBufferIndexOffset, count, data);
    }

  static void setConstantBuffersInternal(
      Renderer *r,
      Shader *shader,
      xsize index,
      xsize count,
      const ShaderConstantData * const* data);

  static void setResources21(
      Renderer *r,
      Shader *shader,
      xsize index,
      xsize count,
      const Resource * const* data);

  static void setResources33(
      Renderer *r,
      Shader *shader,
      xsize index,
      xsize count,
      const Resource * const* data);

  GLuint shader;
  xuint8 maxSetupResources;

  struct Buffer
    {
    Buffer() : data(0), revision(0) { }
    XGL21ShaderData *data;
    xuint8 revision;
    };

  Eks::Vector<Buffer> _buffers;
  friend class XGLRenderer;
  friend class XGLShaderVariable;
  };

//----------------------------------------------------------------------------------------------------------------------
// VERTEX LAYOUT
//----------------------------------------------------------------------------------------------------------------------
class XGLVertexLayout
  {
public:
  bool init1(GLRendererImpl *r, const ShaderVertexLayoutDescription *descs, xsize count)
    {
    _renderer = r;
    xAssert(count < X_UINT8_SENTINEL);
    _attrCount = (xuint8)count;
    xCompileTimeAssert(4 == ShaderVertexLayoutDescription::SemanticCount);
    xAssert(count < ShaderVertexLayoutDescription::SemanticCount)

    vertexSize = 0;
    for(xsize i = 0; i < count; ++i)
      {
      const ShaderVertexLayoutDescription &desc = descs[i];
      Attribute &attr = _attrs[i];

      xAssert(desc.offset < X_UINT8_SENTINEL || desc.offset == ShaderVertexLayoutDescription::OffsetPackTight);
      attr.offset = (xuint8)desc.offset;
      attr.semantic = desc.semantic;
      if(desc.offset == ShaderVertexLayoutDescription::OffsetPackTight)
        {
        attr.offset = (xuint8)vertexSize;
        }

      xCompileTimeAssert(ShaderVertexLayoutDescription::FormatFloat1 == 0);
      xCompileTimeAssert(ShaderVertexLayoutDescription::FormatFloat2 == 1);
      xCompileTimeAssert(ShaderVertexLayoutDescription::FormatFloat3 == 2);
      xCompileTimeAssert(ShaderVertexLayoutDescription::FormatFloat4 == 3);
      attr.components = desc.format + 1;
      xAssert(attr.components <= 4);

      xAssert(vertexSize < X_UINT8_SENTINEL);
      vertexSize = std::max(vertexSize, (xuint8)(attr.offset + attr.size()));
      }

    return true;
    }

  bool init2(GLRendererImpl *, XGLShader* shader)
    {
    const char *semanticNames[] =
    {
      "position",
      "colour",
      "textureCoordinate",
      "normal"
    };
    xCompileTimeAssert(4 == ShaderVertexLayoutDescription::SemanticCount);

    for(GLuint i = 0; i < (GLuint)_attrCount; ++i)
      {
      const Attribute &attr = _attrs[i];
      xsize idx = attr.semantic;

      glBindAttribLocation(shader->shader, i, semanticNames[idx]) GLE;
      }

    return true;
    }

  xuint8 vertexSize;
  struct Attribute
    {
    xuint8 offset;
    xuint8 components;
    xuint8 semantic;
    // type is currently always float.

    inline xsize size() const
      {
      return components * sizeof(float);
      }
    };

  Attribute _attrs[ShaderVertexLayoutDescription::SemanticCount];
  xuint8 _attrCount;
  Eks::GLRendererImpl* _renderer;

  void bind(const XGLGeometryCache *cache) const
    {
    xAssert(cache->_elementSize == vertexSize);
    for(GLuint i = 0, s = (GLuint)_attrCount; i < s; ++i)
      {
      const Attribute &attr = _attrs[i];

      xsize offset = (xsize)attr.offset;

      glEnableVertexAttribArray(i) GLE;
      glVertexAttribPointer(
            i,
            attr.components,
            GL_FLOAT,
            GL_FALSE,
            vertexSize,
            (GLvoid*)offset) GLE;
      }
    }

  void unbind() const
    {
    for(GLuint i = 0, s = (GLuint)_attrCount; i < s; ++i)
      {
      glDisableVertexAttribArray(i) GLE;
      }
    }
  };

//----------------------------------------------------------------------------------------------------------------------
// SHADER DATA
//----------------------------------------------------------------------------------------------------------------------
class XGL21ShaderData
  {
public:
  bool init(GLRendererImpl *, ShaderConstantDataDescription *desc, xsize descCount, const void *data);

  static void update(Renderer *r, ShaderConstantData *, void *data);

  static bool create(
      Renderer *r,
      ShaderConstantData *d,
      ShaderConstantDataDescription *desc,
      xsize descCount,
      const void *data)
    {
    XGL21ShaderData *glD = d->create<XGL21ShaderData>();
    return glD->init(GL_REND(r), desc, descCount, data);
    }

  void bind(xuint32 program, xuint32 index) const;

  typedef void (*BindFunction)(xuint32 location, const xuint8* data);
  struct Binder
    {
    Eks::String name;
    BindFunction bind;
    xsize offset;
    };

  Vector<xuint8> _data;
  Vector<Binder> _binders;
  xuint8 _revision;

  friend class XGLRenderer;
  };

class XGL33ShaderData : public XGLBuffer
  {
public:
  bool init(GLRendererImpl *, ShaderConstantDataDescription *desc, xsize descCount, const void *data);

  static void update(Renderer *r, ShaderConstantData *, void *data);

  static bool create(
      Renderer *r,
      ShaderConstantData *d,
      ShaderConstantDataDescription *desc,
      xsize descCount,
      const void *data)
    {
    XGL33ShaderData *glD = d->create<XGL33ShaderData>();
    return glD->init(GL_REND(r), desc, descCount, data);
    }

  void bind(xuint32 program, xuint32 index) const;

  xsize _size;

  friend class XGLRenderer;
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
    case RasteriserState::CullFront:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      break;
    case RasteriserState::CullBack:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      break;
    case RasteriserState::CullNone:
    default:
      glDisable(GL_CULL_FACE);
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

GLRendererImpl::GLRendererImpl(const detail::RendererFunctions &fns, Eks::AllocatorBase *alloc)
  : _allocator(alloc),
    _modelDataDirty(true),
    _viewDataDirty(true),
    _context(0),
    _currentShader(0),
    _vertexLayout(0),
    _currentFramebuffer(0)
  {
  _modelData.model = Eks::Matrix4x4::Identity();
  setFunctions(fns);
  }


void GLRendererImpl::debugRenderLocator(Renderer *r, RendererDebugLocatorMode m)
  {
  if((m&RendererDebugLocatorMode::DebugLocatorClearShader) != 0)
    {
    GL_REND(r)->_currentShader = 0;
    glUseProgram(0);
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

  glEnableVertexAttribArray(0) GLE;
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 3, lineData) GLE;
  glDrawArrays(GL_LINES, 0, 6) GLE;
  glDisableVertexAttribArray(0);

  float triData[] =
  {
    -0.5, 0, 0,
    0, 0, 0,
    0, -0.5, 0,
    0, 0, 0,
    -0.5, 0, 0,
    0, -0.5, 0,
  };

  glEnableVertexAttribArray(0) GLE;
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 3, triData) GLE;
  glDrawArrays(GL_TRIANGLES, 0, 6) GLE;
  glDisableVertexAttribArray(0);
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

void GLRendererImpl::updateViewData()
  {
  if(_viewDataDirty)
    {
    _view.update(&_viewData);
    _modelDataDirty = true;
    }

  if(_modelDataDirty)
    {
    _modelData.modelView = _viewData.view * _modelData.model;
    _modelData.modelViewProj = _viewData.proj * _modelData.modelView;
    _model.update(&_modelData);
    }

  ShaderConstantData *data[] =
  {
    &_model,
    &_view
  };
  xAssert(_currentShader);
  XGLShader::setConstantBuffersInternal(this, _currentShader, 0, 2, data);
  }

void GLRendererImpl::drawIndexedPrimitive(
    xuint32 primitive,
    Renderer *ren,
    const IndexGeometry *indices,
    const Geometry *vert)
  {
  GLRendererImpl* r = GL_REND(ren);
  xAssert(r->_currentShader);
  xAssert(r->_vertexLayout);
  xAssert(indices);
  xAssert(vert);

  const XGLIndexGeometryCache *idx = indices->data<XGLIndexGeometryCache>();
  const XGLGeometryCache *gC = vert->data<XGLGeometryCache>();


  r->updateViewData();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx->_buffer) GLE;
  glBindBuffer(GL_ARRAY_BUFFER, gC->_buffer) GLE;

  XGLVertexLayout *l = r->_vertexLayout->data<XGLVertexLayout>();
  l->bind(gC);

  glDrawElements(primitive, idx->_indexCount, idx->_indexType, (GLvoid*)((char*)NULL)) GLE;
  l->unbind();

  glBindBuffer(GL_ARRAY_BUFFER, 0) GLE;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) GLE;
  }

void GLRendererImpl::drawIndexedTriangles(
    Renderer *ren,
    const IndexGeometry *indices,
    const Geometry *vert)
  {
  drawIndexedPrimitive(GL_TRIANGLES, ren, indices, vert);
  }

void GLRendererImpl::drawIndexedLines(
    Renderer *ren,
    const IndexGeometry *indices,
    const Geometry *vert)
  {
  drawIndexedPrimitive(GL_LINES, ren, indices, vert);
  }

void GLRendererImpl::drawPrimitive(xuint32 primitive, Renderer *ren, const Geometry *vert)
  {
  GLRendererImpl* r = GL_REND(ren);
  xAssert(r->_currentShader);
  xAssert(r->_vertexLayout);
  xAssert(vert);

  const XGLGeometryCache *gC = vert->data<XGLGeometryCache>();

  r->updateViewData();

  glBindBuffer( GL_ARRAY_BUFFER, gC->_buffer ) GLE;

  XGLVertexLayout *l = r->_vertexLayout->data<XGLVertexLayout>();
  l->bind(gC);

  glDrawArrays( primitive, 0, gC->_elementCount) GLE;
  l->unbind();

  glBindBuffer( GL_ARRAY_BUFFER, 0 ) GLE;
  }

void GLRendererImpl::drawTriangles(Renderer *ren, const Geometry *vert)
  {
  drawPrimitive(GL_TRIANGLES, ren, vert);
  }

void GLRendererImpl::drawLines(Renderer *ren, const Geometry *vert)
  {
  drawPrimitive(GL_LINES, ren, vert);
  }


Shader *GLRendererImpl::stockShader(Renderer *r, RendererShaderType t, const ShaderVertexLayout **l)
  {
  xAssert(l);
  xAssert(GL_REND(r)->stockLayouts[t]);
  xAssert(GL_REND(r)->stockShaders[t]);

  *l = GL_REND(r)->stockLayouts[t];
  return GL_REND(r)->stockShaders[t];
  }

void GLRendererImpl::setStockShader(
    Renderer *r,
    RendererShaderType t,
    Shader *s,
    const ShaderVertexLayout *l)
  {
  GL_REND(r)->stockShaders[t] = s;
  GL_REND(r)->stockLayouts[t] = l;
  }

detail::RendererFunctions gl21fns =
{
  {
    XGL21Framebuffer::create,
    XGLGeometryCache::create,
    XGLIndexGeometryCache::create,
    XGLTexture2D::create,
    XGLShader::create,
    XGLShaderComponent::createVertex,
    XGLShaderComponent::createFragment,
    XGLRasteriserState::create,
    XGLDepthStencilState::create,
    XGLBlendState::create,
    XGL21ShaderData::create
  },
  {
    destroy<FrameBuffer, XGL21Framebuffer>,
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
    destroy<ShaderConstantData, XGL21ShaderData>
  },
  {
    GLRendererImpl::setClearColour,
    XGL21ShaderData::update,
    GLRendererImpl::setViewTransform,
    GLRendererImpl::setProjectionTransform,
    XGLShader::setConstantBuffers,
    XGLShader::setResources21,
    XGLShader::bind,
    XGLRasteriserState::bind,
    XGLDepthStencilState::bind,
    XGLBlendState::bind,
    GLRendererImpl::setTransform,
    GLRendererImpl::setStockShader
  },
  {
    XGLTexture2D::getInfo,
    GLRendererImpl::stockShader,
  },
  {
    GLRendererImpl::drawIndexedTriangles,
    GLRendererImpl::drawTriangles,
    GLRendererImpl::drawIndexedLines,
    GLRendererImpl::drawLines,
    GLRendererImpl::debugRenderLocator
  },
  {
    XGL21Framebuffer::clear,
    XGL21Framebuffer::resize,
    XGL21Framebuffer::beginRender,
    XGL21Framebuffer::endRender,
    XGL21Framebuffer::present,
    XGL21Framebuffer::getTexture
  }
};

detail::RendererFunctions gl33fns =
{
  {
    XGL33Framebuffer::create,
    XGLGeometryCache::create,
    XGLIndexGeometryCache::create,
    XGLTexture2D::create,
    XGLShader::create,
    XGLShaderComponent::createVertex,
    XGLShaderComponent::createFragment,
    XGLRasteriserState::create,
    XGLDepthStencilState::create,
    XGLBlendState::create,
    XGL33ShaderData::create
  },
  {
    destroy<FrameBuffer, XGL33Framebuffer>,
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
    destroy<ShaderConstantData, XGL33ShaderData>
  },
  {
    GLRendererImpl::setClearColour,
    XGL33ShaderData::update,
    GLRendererImpl::setViewTransform,
    GLRendererImpl::setProjectionTransform,
    XGLShader::setConstantBuffers,
    XGLShader::setResources21,
    XGLShader::bind,
    XGLRasteriserState::bind,
    XGLDepthStencilState::bind,
    XGLBlendState::bind,
    GLRendererImpl::setTransform
  },
  {
    XGLTexture2D::getInfo,
    GLRendererImpl::stockShader,
  },
  {
    GLRendererImpl::drawIndexedTriangles,
    GLRendererImpl::drawTriangles,
    GLRendererImpl::drawIndexedLines,
    GLRendererImpl::drawLines,
    GLRendererImpl::debugRenderLocator
  },
  {
    XGL33Framebuffer::clear,
    XGL33Framebuffer::resize,
    XGL33Framebuffer::beginRender,
    XGL33Framebuffer::endRender,
    XGL33Framebuffer::present,
    XGL33Framebuffer::getTexture
  }
};

Renderer *GLRenderer::createGLRenderer(ScreenFrameBuffer *buffer, bool gles, Eks::AllocatorBase* alloc)
  {
#ifdef USE_GLEW
  glewInit();
#endif

  const char* ven = (const char *)glGetString(GL_VENDOR);
  const char* ver = (const char *)glGetString(GL_VERSION);
  qDebug() << "GL Vendor:" << ven << ver;

  xint32 major = 0;
  const char* verPt = ver;
  while(*verPt >= '0' && *verPt < '9')
    {
    xint32 num = *verPt - '0';
    major = (major*10) + num;

    ++verPt;
    }

  if(major < 2)
    {
    return nullptr;
    }

  const detail::RendererFunctions &fns = gl21fns;
  //... if gl33...

  GLRendererImpl *r = alloc->create<GLRendererImpl>(fns, alloc);



  glEnable( GL_DEPTH_TEST ) GLE;
  GLRendererImpl::setClearColour(r, Colour(0.0f, 0.0f, 0.0f, 1.0f));

  XGLFramebuffer* fb = buffer->create<XGLFramebuffer>();
  fb->init(r);
  buffer->setRenderer(r);

  ShaderConstantDataDescription modelDesc[] =
  {
    { "model", ShaderConstantDataDescription::Matrix4x4 },
    { "modelView", ShaderConstantDataDescription::Matrix4x4 },
    { "modelViewProj", ShaderConstantDataDescription::Matrix4x4 },
  };
  ShaderConstantDataDescription viewDesc[] =
  {
    { "view", ShaderConstantDataDescription::Matrix4x4 },
    { "proj", ShaderConstantDataDescription::Matrix4x4 },
  };

  ShaderConstantData::delayedCreate(r->_model, r, modelDesc, X_ARRAY_COUNT(modelDesc));
  ShaderConstantData::delayedCreate(r->_view, r, viewDesc, X_ARRAY_COUNT(viewDesc));

  return r;
  }

void GLRenderer::destroyGLRenderer(Renderer *r, ScreenFrameBuffer *buffer, Eks::AllocatorBase* alloc)
  {
  buffer->destroy<XGLFramebuffer>();
  buffer->setRenderer(0);

  alloc->destroy(GL_REND(r));
  }

//----------------------------------------------------------------------------------------------------------------------
// TEXTURE
//----------------------------------------------------------------------------------------------------------------------
bool XGLTexture2D::init(GLRendererImpl *, xuint32 format, xsize width, xsize height, const void *data)
  {
  _type = GL_TEXTURE_2D;

  size = VectorUI2D((xuint32)width, (xuint32)height);

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
  #ifdef USE_GLEW
    GL_DEPTH_COMPONENT24
  #else
  # ifdef USE_GLES
    GL_DEPTH_COMPONENT16
  # endif
  #endif
  };
  xCompileTimeAssert(X_ARRAY_COUNT(formatMap) == TextureFormatCount);

  // 0 at end could be data to unsigned byte...
  glTexImage2D(GL_TEXTURE_2D, 0, formatMap[format], (GLsizei)width, (GLsizei)height, 0, formatMap[format], GL_UNSIGNED_BYTE, data) GLE;

  glBindTexture(GL_TEXTURE_2D, 0) GLE;

  return true;
  }

XGLTexture2D::~XGLTexture2D()
  {
  clear();
  }

void XGLTexture2D::getInfo(const Renderer *, const Texture2D *tex, Eks::VectorUI2D& v)
  {
  const XGLTexture2D *tImpl = tex->data<XGLTexture2D>();
  v = tImpl->size;
  }

void XGLTexture2D::clear()
  {
  glDeleteTextures(1, &_id) GLE;
  }

//----------------------------------------------------------------------------------------------------------------------
// FRAMEBUFFER
//----------------------------------------------------------------------------------------------------------------------
bool XGL21Framebuffer::init(Renderer *r, TextureFormat cF, TextureFormat dF, xuint32 width, xuint32 height)
  {
  GLRendererImpl *impl = GL_REND(r);

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
  return isValid(impl);
  }

bool XGL33Framebuffer::init(Renderer *r, TextureFormat cF, TextureFormat dF, xuint32 width, xuint32 height)
  {
  GLRendererImpl *impl = GL_REND(r);

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
  return isValid(impl);
  }

bool XGLFramebuffer::init(GLRendererImpl *r)
  {
  _buffer = 0;
  _impl = r;
  return true;
  }

XGL21Framebuffer::~XGL21Framebuffer( )
  {
  if( _buffer )
    {
    glDeleteFramebuffers( 1, &_buffer ) GLE;
    }
  }

XGL33Framebuffer::~XGL33Framebuffer( )
  {
  if( _buffer )
    {
    glDeleteFramebuffers( 1, &_buffer ) GLE;
    }
  }

bool XGL21Framebuffer::isValid(GLRendererImpl *) const
  {
  if(!_buffer)
    {
    return true;
    }

  glBindFramebuffer( GL_FRAMEBUFFER, _buffer ) GLE;
  int status = glCheckFramebufferStatus(GL_FRAMEBUFFER) GLE;

  if( status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT )
    {
    qWarning() << "Framebuffer Incomplete attachment";
    }
  else if( status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
    {
    qWarning() << "Framebuffer Incomplete missing attachment";
    }
  else if( status == GL_FRAMEBUFFER_UNSUPPORTED )
    {
    qWarning() << "Framebuffer unsupported attachment";
    }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 ) GLE;

  return status == GL_FRAMEBUFFER_COMPLETE;
  }

bool XGL33Framebuffer::isValid(GLRendererImpl *) const
  {
  if(!_buffer)
    {
    return true;
    }

  glBindFramebuffer( GL_FRAMEBUFFER, _buffer ) GLE;
  int status = glCheckFramebufferStatus(GL_FRAMEBUFFER) GLE;

  if( status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT )
    {
    qWarning() << "Framebuffer Incomplete attachment";
    }
  else if( status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
    {
    qWarning() << "Framebuffer Incomplete missing attachment";
    }
  else if( status == GL_FRAMEBUFFER_UNSUPPORTED )
    {
    qWarning() << "Framebuffer unsupported attachment";
    }

  glBindFramebuffer( GL_FRAMEBUFFER, 0 ) GLE;

  return status == GL_FRAMEBUFFER_COMPLETE;
  }

void XGL21Framebuffer::bind(GLRendererImpl *r)
  {
  xAssert( isValid(r) );
  glBindFramebuffer( GL_FRAMEBUFFER, _buffer ) GLE;
  }

void XGL33Framebuffer::bind(GLRendererImpl *r)
  {
  xAssert( isValid(r) );
  glBindFramebuffer( GL_FRAMEBUFFER, _buffer ) GLE;
  }

void XGL21Framebuffer::unbind(GLRendererImpl *)
  {
  glBindFramebuffer( GL_FRAMEBUFFER, 0 ) GLE;
  }

void XGL33Framebuffer::unbind(GLRendererImpl *)
  {
  glBindFramebuffer( GL_FRAMEBUFFER, 0 ) GLE;
  }

//----------------------------------------------------------------------------------------------------------------------
// SHADER COMPONENT
//----------------------------------------------------------------------------------------------------------------------
bool XGLShaderComponent::init(GLRendererImpl *impl, xuint32 type, const char *data, xsize size)
  {
#ifdef USE_GLEW
  xAssert(glCreateShader);
#endif
  _component = glCreateShader(type) GLE;

  const char *extra = "#define X_GLSL_VERSION 120\n";

  int lengths[] =
    {
    (int)strlen(extra),
    (int)size,
    };

  const char *strs[] =
    {
    extra,
    data,
    };

  glShaderSource(_component, X_ARRAY_COUNT(lengths), strs, lengths) GLE;
  glCompileShader(_component) GLE;

  int infoLogLength = 0;
  glGetShaderiv(_component, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 0)
    {
    Eks::String infoLog(impl->_allocator);
    infoLog.resize(infoLogLength, '\0');
    int charsWritten  = 0;
    glGetShaderInfoLog(_component, infoLogLength, &charsWritten, infoLog.data());
    qDebug() << infoLog.toQString();
    }

  int success = 0;
  glGetShaderiv(_component, GL_COMPILE_STATUS, &success);

  return success;
  }

bool XGLShaderComponent::createVertex(
    Renderer *r,
    ShaderVertexComponent *v,
    const char *s,
    xsize l,
    const ShaderVertexLayoutDescription *vertexDescriptions,
    xsize vertexItemCount,
    ShaderVertexLayout *layout)
  {
  XGLShaderComponent *glS = v->create<XGLShaderComponent>();
  if(!glS->init(GL_REND(r), GL_VERTEX_SHADER, s, l))
    {
    return false;
    }

  glS->_layout = 0;
  if(layout)
    {
    XGLVertexLayout *glL = layout->create<XGLVertexLayout>();
    glS->_layout = glL;
    xAssert(vertexItemCount > 0);
    xAssert(vertexDescriptions);

    return glL->init1(GL_REND(r), vertexDescriptions, vertexItemCount);
    }

  return true;
  }

//----------------------------------------------------------------------------------------------------------------------
// SHADER DATA
//----------------------------------------------------------------------------------------------------------------------
bool XGL21ShaderData::init(
    GLRendererImpl *r,
    ShaderConstantDataDescription* desc,
    xsize descCount,
    const void *data)
  {
  _revision = 0;
  _data.allocator() = TypedAllocator<xuint8>(r->_allocator);
  _binders.allocator() = TypedAllocator<Binder>(r->_allocator);

  struct Type
    {
    BindFunction bind;
    xsize size;

    static void bindFloat(xuint32 location, const xuint8 *data8)
      {
      glUniform1fv(location, 1, (const float*)data8);
      }

    static void bindFloat3(xuint32 location, const xuint8 *data8)
      {
      glUniform3fv(location, 1, (const float*)data8);
      }

    static void bindFloat4(xuint32 location, const xuint8 *data8)
      {
      glUniform4fv(location, 1, (const float*)data8);
      }

    static void bindMat4x4(xuint32 location, const xuint8 *data8)
      {
      glUniformMatrix4fv(location, 1, false, (const float*)data8);
      }
    };

  Type typeMap[] =
  {
    { Type::bindFloat, sizeof(float) },
    { Type::bindFloat3, sizeof(float) * 3 },
    { Type::bindFloat4, sizeof(float) * 4 },
    { Type::bindMat4x4, sizeof(float) * 16 },
  };
  xCompileTimeAssert(X_ARRAY_COUNT(typeMap) == ShaderConstantDataDescription::TypeCount);

  xsize size = 0;
  _binders.resize(descCount);
  for(xsize i = 0; i < descCount; ++i)
    {
    const ShaderConstantDataDescription &description = desc[i];
    const Type &type = typeMap[description.type];

    Binder &b = _binders[i];
    b.name = Eks::String(description.name, r->_allocator);
    b.offset = size;
    b.bind = type.bind;

    size += type.size;
    }

  if(data)
    {
    _data.resizeAndCopy(size, (xuint8*)data);
    }
  else
    {
    _data.resize(size, 0);
    }

  return true;
  }

void XGL21ShaderData::update(Renderer *, ShaderConstantData *constant, void *data)
  {
  XGL21ShaderData* sData = constant->data<XGL21ShaderData>();

  memcpy(sData->_data.data(), data, sData->_data.size());
  ++sData->_revision;
  }

void XGL21ShaderData::bind(xuint32 program, xuint32 index) const
  {
  char str[256];
#ifdef Q_OS_WIN
  xsize pos = sprintf_s(str, X_ARRAY_COUNT(str), "cb%d.", index);
#else
  xsize pos = sprintf(str, "cb%d.", index);
#endif

  const xuint8* data = _data.data();
  xForeach(const Binder &b, _binders)
    {
    xsize strl = b.name.length();
    memcpy(str + pos, b.name.data(), strl);
    str[strl + pos] = '\0';

    xint32 location = glGetUniformLocation(program, str);
    if(location != -1)
      {
      b.bind(location, data + b.offset);
      }
    }
  }

#ifdef USE_GLEW
bool XGL33ShaderData::init(
    GLRendererImpl *r,
    ShaderConstantDataDescription *desc,
    xsize descCount,
    const void *data)
  {
  (void)descCount;
  (void)desc;
  xAssertFail();
  xsize size = 0;
  _size = size;
  return XGLBuffer::init(r, data, GL_UNIFORM_BUFFER, GL_STREAM_DRAW, size);
  }

void XGL33ShaderData::update(Renderer *, ShaderConstantData *constant, void *data)
  {
  XGL33ShaderData *c = constant->data<XGL33ShaderData>();

  glBindBuffer(GL_UNIFORM_BUFFER, c->_buffer);
  glBufferData(GL_UNIFORM_BUFFER, c->_size, data, GL_STREAM_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

void XGL33ShaderData::bind(xuint32 program, xuint32 index) const
  {
  (void)program;
  glBindBufferRange(GL_UNIFORM_BUFFER, index, _buffer, 0, _size);
  }
#endif

//----------------------------------------------------------------------------------------------------------------------
// SHADER
//----------------------------------------------------------------------------------------------------------------------
XGLShader::~XGLShader()
  {
  glDeleteProgram(shader);
  }

bool XGLShader::init( GLRendererImpl *impl, XGLShaderComponent *v, XGLShaderComponent *f)
  {
  _buffers.allocator() = TypedAllocator<Buffer>(impl->_allocator);
  maxSetupResources = 0;
  shader = glCreateProgram();
  glAttachShader(shader, v->_component);
  glAttachShader(shader, f->_component);

  xAssert(!f->_layout);
  if(v->_layout)
    {
    if(!v->_layout->init2(impl, this))
      {
      return false;
      }
    }

  glLinkProgram(shader);

  int infologLength = 0;

  glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 0)
    {
    Eks::String infoLog(impl->_allocator);
    infoLog.resize(infologLength, '\0');
    int charsWritten  = 0;
    glGetProgramInfoLog(shader, infologLength, &charsWritten, infoLog.data());
    qDebug() << infoLog.toQString();
    }

  int success = 0;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if(!success)
    {
    return false;
    }

  return true;
  }

void XGLShader::bind(Renderer *ren, const Shader *shader, const ShaderVertexLayout *layout)
  {
  GLRendererImpl* r = GL_REND(ren);
  if( shader &&
      ( r->_currentShader == 0 || r->_currentShader != shader || r->_vertexLayout != layout) )
    {
    r->_currentShader = const_cast<Shader *>(shader);
    r->_vertexLayout = const_cast<ShaderVertexLayout *>(layout);
    XGLShader* shaderInt = r->_currentShader->data<XGLShader>();

    glUseProgram(shaderInt->shader);
    }
  else if( shader == 0 && r->_currentShader != 0 )
    {
    glUseProgram(0);
    r->_currentShader = 0;
    r->_vertexLayout = 0;
    }
  }

#if 0
void XGLShader::setConstantBuffers(
    Renderer *,
    Shader *shader,
    xsize index,
    xsize count,
    const ShaderConstantData * const* data)
  {
  XGLShader* shaderImpl = shader->data<XGLShader>();
  for(xsize i = 0; i < count; ++i)
    {
    xsize blockIndex = i + index + GLRendererImpl::ConstantBufferIndexOffset;
    const XGL21ShaderData* sImpl = data[i]->data<XGL21ShaderData>();

    glUniformBlockBinding(shaderImpl->shader, blockIndex, blockIndex);
    sImpl->bind(blockIndex);
    }
  }
#endif

void XGLShader::setConstantBuffersInternal(
    Renderer *r,
    Shader *s,
    xsize index,
    xsize count,
    const ShaderConstantData * const* data)
  {
  XGLShader* shader = s->data<XGLShader>();

  if(GL_REND(r)->_currentShader != s)
    {
    glUseProgram(shader->shader);
    GL_REND(r)->_currentShader = 0;
    }

  if(count > shader->_buffers.size())
    {
    shader->_buffers.resize(count, Buffer());
    }

  for(xuint32 i = 0; i < (xuint32)count; ++i)
    {
    const ShaderConstantData *cb = data[i];
    const XGL21ShaderData* cbImpl = cb->data<XGL21ShaderData>();

    Buffer &buf = shader->_buffers[i];

    if(buf.data != cbImpl || buf.revision != cbImpl->_revision)
      {
      cbImpl->bind(shader->shader, i + (xuint32)index);
      buf.revision = cbImpl->_revision;
      }
    }
  }

void XGLShader::setResources21(
    Renderer *,
    Shader *s,
    xsize index,
    xsize count,
    const Resource * const* data)
  {
  XGLShader* shader = s->data<XGLShader>();
  for(GLuint i = shader->maxSetupResources; i < (GLuint)count; ++i)
    {
    char str[256];
  #ifdef Q_OS_WIN
    sprintf_s(str, X_ARRAY_COUNT(str), "rsc%d", (int)index);
  #else
    sprintf(str, "rsc%d", (int)index);
  #endif

    xint32 location = glGetUniformLocation(shader->shader, str);

    if(location != -1)
      {
      glUniform1i(location, i);
      }
    }

  for(GLuint i = 0; i < (GLuint)count; ++i)
    {
    const Resource *rsc = data[i];
    const XGLShaderResource* rscImpl = rsc->data<XGLShaderResource>();
    rscImpl->bindResource(i+(GLuint)index);
    }
  }

//----------------------------------------------------------------------------------------------------------------------
// BUFFER
//----------------------------------------------------------------------------------------------------------------------
bool XGLBuffer::init( GLRendererImpl *, const void *data, xuint32 type, xuint32 renderType, xsize size)
  {
  glGenBuffers(1, &_buffer);

  glBindBuffer(type, _buffer) GLE;
  glBufferData(type, size, data, renderType) GLE;
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
  _indexCount = (GLuint)elementCount;

  xsize dataSize = elementCount * typeMap[type].size;
  return XGLBuffer::init(r, data, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, dataSize);
  }

//----------------------------------------------------------------------------------------------------------------------
// GEOMETRY CACHE
//----------------------------------------------------------------------------------------------------------------------

bool XGLGeometryCache::init(GLRendererImpl *r, const void *data, xsize elementSize, xsize elementCount)
  {
  xsize dataSize = elementSize * elementCount;
  _elementCount = (GLuint)elementCount;
  _elementSize = (GLuint)elementSize;
  return XGLBuffer::init(r, data, GL_ARRAY_BUFFER, GL_STATIC_DRAW, dataSize);
  }

}

#endif
