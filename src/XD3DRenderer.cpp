#include <d3d11.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>
#include "XD3DRenderer.h"
#include "XGeometry.h"
#include "XFramebuffer.h"
#include "XTexture.h"
#include "XD3DRendererImpl.h"
#include "XRenderer.h"
#include "XColour"
#include "XOptional"
#include "XShader.h"

using namespace DirectX;

namespace Eks
{

#define D3D(r) static_cast<D3DRendererImpl*>(r)

void clear(Renderer *r, FrameBuffer *buffer, xuint32 clear)
  {
  XD3DFrameBufferImpl *fb = buffer->data<XD3DFrameBufferImpl>();
  fb->clear(
        D3D(r)->_d3dContext.Get(),
        (clear&FrameBuffer::ClearColour) != 0,
        (clear&FrameBuffer::ClearDepth) != 0,
        D3D(r)->_clearColour.data(),
        1.0f,
        0);
  }

Texture2D *getFramebufferTexture(Renderer *, FrameBuffer *f, xuint32 mode)
  {
  XD3DFrameBufferImpl *fb = f->data<XD3DFrameBufferImpl>();

  if(mode == FrameBuffer::TextureColour)
    {
    return &fb->colour;
    }
  else if(mode == FrameBuffer::TextureDepthStencil)
    {
    return &fb->colour;
    }

  return 0;
  }

void beginFrame(Renderer *r, FrameBuffer *buffer)
  {
  if(D3D(r)->_updateWorldTransformData)
    {
    D3D(r)->_worldTransformData.update(D3D(r)->_d3dContext.Get());
    }

  XD3DFrameBufferImpl* framebuffer = buffer->data<XD3DFrameBufferImpl>();

  clear(r, buffer, FrameBuffer::ClearColour|FrameBuffer::ClearDepth);
  D3D(r)->setRenderTarget(framebuffer);
  }

void endFrame(Renderer *, FrameBuffer *)
  {
  }

void present(Renderer *r, ScreenFrameBuffer *screenBuffer, bool *deviceListOptional)
  {
  XD3DSwapChainImpl *swap = screenBuffer->data<XD3DSwapChainImpl>();
  swap->present(D3D(r)->_d3dContext.Get(), deviceListOptional);
  }

bool resize(Renderer *r, ScreenFrameBuffer *screenBuffer, xuint32 w, xuint32 h, xuint32 rotation)
  {
  XD3DSwapChainImpl *swap = screenBuffer->data<XD3DSwapChainImpl>();
  return D3D(r)->resize(swap, w, h, rotation);
  }

void setTransform(Renderer *r, const Transform &tr)
  {
  D3D(r)->_modelTransformData.data = tr.matrix();
  D3D(r)->_modelTransformData.update(D3D(r)->_d3dContext.Get());
  }

void setClearColour(Renderer *r, const Colour &col)
  {
  D3D(r)->_clearColour = col;
  }

bool createFramebuffer(
    Renderer *r,
    FrameBuffer *buffer,
    xuint32 w,
    xuint32 h,
    xuint32 colourFormat,
    xuint32 depthFormat)
  {
  xAssertFail();
  return false;
  }

bool createShader(Renderer *, Shader *s, ShaderVertexComponent *v, ShaderFragmentComponent *f)
  {
  XD3DFragmentShaderImpl *frag = f->data<XD3DFragmentShaderImpl>();
  XD3DVertexShaderImpl *vert = v->data<XD3DVertexShaderImpl>();

  XD3DSurfaceShaderImpl* shd = s->create<XD3DSurfaceShaderImpl>();

  shd->_pixelShader = frag->_pixelShader;
  shd->_vertexShader = vert->_vertexShader;

  return shd->_pixelShader && shd->_vertexShader;
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
  XD3DVertexShaderImpl *vert = v->create<XD3DVertexShaderImpl>();

  if(!vert->create(D3D(r)->_d3dDevice.Get(), s, l))
    {
    return false;
    }

  if(vertexDescriptions)
    {
    xAssert(layout);
    xAssert(vertexItemCount);

    xCompileTimeAssert(D3D11_APPEND_ALIGNED_ELEMENT == X_SIZE_SENTINEL);
    xCompileTimeAssert(D3D11_INPUT_PER_VERTEX_DATA == ShaderVertexLayoutDescription::Slot::PerVertex);
    xCompileTimeAssert(D3D11_INPUT_PER_INSTANCE_DATA == ShaderVertexLayoutDescription::Slot::PerInstance);

    const char* semanticMap[] =
    {
      "POSITION",
      "COLOUR",
      "TEXCOORD",
      "NORMAL"
    };
    xCompileTimeAssert(X_ARRAY_COUNT(semanticMap) == ShaderVertexLayoutDescription::SemanticCount);


    const DXGI_FORMAT formatMap[] =
    {
      DXGI_FORMAT_R32_FLOAT,
      DXGI_FORMAT_R32G32_FLOAT,
      DXGI_FORMAT_R32G32B32_FLOAT,
      DXGI_FORMAT_R32G32B32A32_FLOAT
    };
    xCompileTimeAssert(X_ARRAY_COUNT(formatMap) == ShaderVertexLayoutDescription::FormatCount);

    D3D11_INPUT_ELEMENT_DESC *vertexDesc =
        (D3D11_INPUT_ELEMENT_DESC*)alloca(sizeof(D3D11_INPUT_ELEMENT_DESC)*vertexItemCount);
    D3D11_INPUT_ELEMENT_DESC *currentVertexDesc = vertexDesc;
    for(xsize i = 0; i < vertexItemCount; ++i, ++currentVertexDesc, ++vertexDescriptions)
      {
      currentVertexDesc->SemanticName = semanticMap[vertexDescriptions->semantic];
      currentVertexDesc->SemanticIndex = 0; // increase for matrices...
      currentVertexDesc->Format = formatMap[vertexDescriptions->format]; // increase for matrices...
      currentVertexDesc->AlignedByteOffset = vertexDescriptions->offset;

      currentVertexDesc->InputSlot = vertexDescriptions->slot.index;
      currentVertexDesc->InputSlotClass = (D3D11_INPUT_CLASSIFICATION)vertexDescriptions->slot.type;
      currentVertexDesc->InstanceDataStepRate = vertexDescriptions->slot.instanceDataStepRate;
      }

    xAssert(!layout->isValid());
    XD3DShaderInputLayout *lay = layout->create<XD3DShaderInputLayout>();
    if(failedCheck(
      D3D(r)->_d3dDevice->CreateInputLayout(
        vertexDesc,
        vertexItemCount,
        s,
        l,
        &lay->_inputLayout
        )))
      {
      return false;
      }
    }

  return true;
  }

bool createFragmentShaderComponent(Renderer *r, ShaderFragmentComponent *f, const char *s, xsize l)
  {
  XD3DFragmentShaderImpl *frag = f->create<XD3DFragmentShaderImpl>();
  return frag->create(D3D(r)->_d3dDevice.Get(), s, l);
  }

bool createGeometry(
    Renderer *r,
    Geometry *g,
    const void *data,
    xsize elementSize,
    xsize elementCount)
  {
  XD3DVertexBufferImpl *geo = g->create<XD3DVertexBufferImpl>();

  xsize dataSize = elementSize * elementCount;

  bool result = geo->create(D3D(r)->_d3dDevice.Get(), data, dataSize, D3D11_BIND_VERTEX_BUFFER);

  geo->elementSize = elementSize;
  geo->elementCount = elementCount;

  return result;
  }

bool createIndexGeometry(
    Renderer *r,
    IndexGeometry *g,
    int type,
    const void *index,
    xsize indexCount)
  {
  XD3DIndexBufferImpl *geo = g->create<XD3DIndexBufferImpl>();

  struct Format
    {
    DXGI_FORMAT format;
    xsize elementSize;
    };

  const Format typeMap[] =
  {
    { DXGI_FORMAT_R16_UINT, sizeof(xuint16) }
  };
  xCompileTimeAssert(X_ARRAY_COUNT(typeMap) == IndexGeometry::TypeCount);
  const Format &typeData = typeMap[type];
  geo->format = typeData.format;

  xsize dataSize = indexCount * typeData.elementSize;
  bool result = geo->create(D3D(r)->_d3dDevice.Get(), index, dataSize, D3D11_BIND_INDEX_BUFFER);

  geo->count = indexCount;

  return result;
  }

bool createTexture(
    Renderer *r,
    Texture2D *t,
    xsize width,
    xsize height,
    xuint32 format,
    void *data)
  {
  XD3DTexture2DImpl *tex = t->create<XD3DTexture2DImpl>();

  struct Format
    {
    DXGI_FORMAT format;
    xuint8 bpp;
    };

  Format formatMap[] =
    {
    { DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(xuint8) * 4 }
    };
  xCompileTimeAssert(X_ARRAY_COUNT(formatMap) == Texture2D::FormatCount);


  return tex->create(
        D3D(r)->_d3dDevice.Get(),
        width,
        height,
        formatMap[format].format,
        data,
        formatMap[format].bpp);
  }

bool createRasteriserState(
    Renderer *r,
    RasteriserState *s,
    xuint32 cull)
  {
  XD3DRasteriserStateImpl *ras = s->create<XD3DRasteriserStateImpl>();

  D3D11_RASTERIZER_DESC1 desc;

  D3D11_CULL_MODE cullMap[] =
  {
    D3D11_CULL_NONE,
    D3D11_CULL_BACK,
    D3D11_CULL_FRONT
  };
  xCompileTimeAssert(X_ARRAY_COUNT(cullMap) == RasteriserState::CullModeCount);

  desc.FillMode = D3D11_FILL_SOLID;
  desc.CullMode = cullMap[cull];
  desc.FrontCounterClockwise = true;
  desc.DepthBias = false;
  desc.DepthBiasClamp = 0;
  desc.SlopeScaledDepthBias = 0;
  desc.DepthClipEnable = true;
  desc.ScissorEnable = false;
  desc.MultisampleEnable = false;
  desc.AntialiasedLineEnable = false;
  desc.ForcedSampleCount = 0;

  return ras->create(D3D(r)->_d3dDevice.Get(), desc);
  }

bool createShaderConstantData(
    Renderer *r,
    ShaderConstantData *s,
    xsize size,
    void *data)
  {
  XD3DBufferImpl *b = s->create<XD3DBufferImpl>();

  enum
    {
    SizeAlignment = 16
    };

  xAssert((size % SizeAlignment) == 0)

  return b->create(D3D(r)->_d3dDevice.Get(), data, size, D3D11_BIND_CONSTANT_BUFFER);
  }

void debugRenderLocator(Renderer *, RendererDebugLocatorMode)
  {
  }

void updateShaderConstantData(Renderer *r, ShaderConstantData *s, void *data)
  {
  XD3DBufferImpl *b = s->data<XD3DBufferImpl>();
  b->update(D3D(r)->_d3dContext.Get(), data);
  }

void drawTriangles(Renderer *r, const Geometry *vert)
  {
  const XD3DVertexBufferImpl *geo = vert->data<XD3DVertexBufferImpl>();

  UINT stride = geo->elementSize;
  UINT offset = 0;
  D3D(r)->_d3dContext->IASetVertexBuffers(
    0,
    1,
    geo->buffer.GetAddressOf(),
    &stride,
    &offset
    );

  D3D(r)->_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  D3D(r)->_d3dContext->Draw(
    geo->elementCount,
    0
    );
  }

void drawIndexedTriangles(Renderer *r, const IndexGeometry *indices, const Geometry *vert)
  {
  const XD3DVertexBufferImpl *geo = vert->data<XD3DVertexBufferImpl>();
  const XD3DIndexBufferImpl *idx = indices->data<XD3DIndexBufferImpl>();

  UINT stride = geo->elementSize;
  UINT offset = 0;
  D3D(r)->_d3dContext->IASetVertexBuffers(
    0,
    1,
    geo->buffer.GetAddressOf(),
    &stride,
    &offset
    );

  D3D(r)->_d3dContext->IASetIndexBuffer(
    idx->buffer.Get(),
    idx->format,
    0
    );

  D3D(r)->_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  D3D(r)->_d3dContext->DrawIndexed(
    idx->count,
    0,
    0
    );
  }

template <typename T, typename U> void destroy(Renderer *, T *g)
  {
  if(g->isValid())
    {
    g->destroy<U>();
    }
  }

void setViewTransform(Renderer *r, const Transform &v)
  {
  D3D(r)->_worldTransformData.data.view = v.matrix().transpose();
  D3D(r)->_updateWorldTransformData = true;
  }

void setProjectionTransform(Renderer *r, const ComplexTransform &p)
  {
  D3D(r)->_worldTransformData.data.projection = p.matrix().transpose();
  D3D(r)->_updateWorldTransformData = true;
  }

void setFragmentShaderConstantBuffer(
    Renderer *r,
    Shader *,
    xsize index,
    xsize num,
    const ShaderConstantData **s)
  {
  ID3D11Buffer **buffers = (ID3D11Buffer **)alloca(sizeof(ID3D11Buffer *) * num);
  for(xsize i = 0; i < num; ++i)
    {
    const XD3DBufferImpl *b = s[i]->data<XD3DBufferImpl>();
    buffers[i] = b->buffer.Get();
    }

  D3D(r)->_d3dContext->PSSetConstantBuffers(
    D3DRendererImpl::UserPSContantBufferOffset + index,
    num,
    buffers
    );
  }

void setVertexShaderConstantBuffer(
    Renderer *r,
    Shader *,
    xsize index,
    xsize num,
    const ShaderConstantData **s)
  {
  ID3D11Buffer **buffers = (ID3D11Buffer **)alloca(sizeof(ID3D11Buffer *) * num);
  for(xsize i = 0; i < num; ++i)
    {
    const XD3DBufferImpl *b = s[i]->data<XD3DBufferImpl>();
    buffers[i] = b->buffer.Get();
    }

  D3D(r)->_d3dContext->VSSetConstantBuffers(
    D3DRendererImpl::UserVSContantBufferOffset + index,
    num,
    buffers
    );
  }

void setFragmentShaderResource(
    Renderer *r,
    Shader *,
    xsize index,
    xsize num,
    const Resource **s)
  {
  ID3D11ShaderResourceView **buffers =
    (ID3D11ShaderResourceView **)alloca(sizeof(ID3D11ShaderResourceView *) * num);
  for(xsize i = 0; i < num; ++i)
    {
    const XD3DShaderResourceImpl *b = s[i]->data<XD3DShaderResourceImpl>();
    buffers[i] = b->view.Get();
    }

  D3D(r)->_d3dContext->PSSetShaderResources(
    D3DRendererImpl::UserPSContantBufferOffset + index,
    num,
    buffers
    );
  }

void setVertexShaderResource(
    Renderer *r,
    Shader *,
    xsize index,
    xsize num,
    const Resource **s)
  {
  ID3D11ShaderResourceView **buffers =
    (ID3D11ShaderResourceView **)alloca(sizeof(ID3D11ShaderResourceView *) * num);
  for(xsize i = 0; i < num; ++i)
    {
    const XD3DShaderResourceImpl *b = s[i]->data<XD3DShaderResourceImpl>();
    buffers[i] = b->view.Get();
    }

  D3D(r)->_d3dContext->VSSetShaderResources(
    D3DRendererImpl::UserVSContantBufferOffset + index,
    num,
    buffers
    );
  }

void setShader(Renderer *r, const Shader *s, const ShaderVertexLayout *layout)
  {
  const XD3DSurfaceShaderImpl* shd = s->data<XD3DSurfaceShaderImpl>();
  shd->bind(D3D(r)->_d3dContext.Get());

  ID3D11Buffer *buffers[] =
  {
    D3D(r)->_worldTransformData.buffer.Get(),
    D3D(r)->_modelTransformData.buffer.Get()
  };

  D3D(r)->_d3dContext->VSSetConstantBuffers(
    0,
    X_ARRAY_COUNT(buffers),
    buffers
    );

  ID3D11SamplerState *smplr[] = { D3D(r)->_sampler._sampler.Get() };
  D3D(r)->_d3dContext->PSSetSamplers(0, 1, smplr);

  const XD3DShaderInputLayout* lay = layout->data<XD3DShaderInputLayout>();
  D3D(r)->_d3dContext->IASetInputLayout(lay->_inputLayout.Get());
  }

void setRasteriserState(Renderer *r, const RasteriserState *s)
  {
  const XD3DRasteriserStateImpl *ras = s->data<XD3DRasteriserStateImpl>();

  D3D(r)->_d3dContext->RSSetState(ras->_state.Get());
  }

Renderer *D3DRenderer::createD3DRenderer(IUnknown *window, ScreenFrameBuffer *buffer)
  {
  detail::RendererFunctions fns =
  {
    {
      createFramebuffer,
      createGeometry,
      createIndexGeometry,
      createTexture,
      createShader,
      createVertexShaderComponent,
      createFragmentShaderComponent,
      createRasteriserState,
      createShaderConstantData
    },
    {
      destroy<FrameBuffer, XD3DFrameBufferImpl>,
      destroy<Geometry, XD3DVertexBufferImpl>,
      destroy<IndexGeometry, XD3DIndexBufferImpl>,
      destroy<Texture2D, XD3DTexture2DImpl>,
      destroy<Shader, XD3DSurfaceShaderImpl>,
      destroy<ShaderVertexLayout, XD3DShaderInputLayout>,
      destroy<ShaderVertexComponent, XD3DVertexShaderImpl>,
      destroy<ShaderFragmentComponent, XD3DFragmentShaderImpl>,
      destroy<RasteriserState, XD3DRasteriserStateImpl>,
      destroy<ShaderConstantData, XD3DBufferImpl>
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

  D3DRendererImpl *r = new D3DRendererImpl(window, fns);

  xAssert(!buffer->isValid());
  buffer->create<XD3DSwapChainImpl>();
  buffer->setRenderer(r);


  return r;
  }

void D3DRenderer::destroyD3DRenderer(Renderer *r, ScreenFrameBuffer *buffer)
  {
  buffer->setRenderer(0);
  destroy<ScreenFrameBuffer, XD3DSwapChainImpl>(r, buffer);
  delete D3D(r);
  }

}
