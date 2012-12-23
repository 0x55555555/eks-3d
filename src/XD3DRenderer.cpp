#include <d3d11.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>
#include "XD3DRenderer.h"
#include "XGeometry.h"
#include "XD3DRendererImpl.h"
#include "XColour"
#include "XOptional"
#include "XShader.h"

using namespace DirectX;

namespace Eks
{

D3DRenderer::D3DRenderer(IUnknown *window)
  {
  _impl = new D3DRendererImpl;

  _impl->_window = window;
  _impl->createResources();
  setClearColour(Colour(0.0f, 0.0f, 0.0f));
  }

D3DRenderer::~D3DRenderer()
  {
  delete _impl;
  _impl = 0;
  }

ID3D11Device1 *D3DRenderer::getD3DDevice()
  {
  return _impl->_d3dDevice.Get();
  }

ID3D11DeviceContext1 *D3DRenderer::getD3DContext()
  {
  return _impl->_d3dContext.Get();
  }

void D3DRenderer::beginFrame()
  {
  if(_impl->_updateWorldTransformData)
    {
    _impl->_worldTransformData.update(_impl->_d3dContext.Get());
    }

  clear();
  _impl->setRenderTarget(&_impl->_renderTarget);
  }

void D3DRenderer::endFrame(bool *deviceListOptional)
  {
  _impl->_renderTarget.present(_impl->_d3dContext.Get(), deviceListOptional);
  }

bool D3DRenderer::resize(xuint32 w, xuint32 h, Rotation rotation)
  {
  return _impl->resize(w, h, rotation);
  }

void D3DRenderer::pushTransform(const Transform &tr)
  {
  xAssert((_impl->_currentTransform - _impl->_transformStack) <
            D3DRendererImpl::TransformStackSize);

  Matrix4x4& oldTransform = *_impl->_currentTransform;
  Matrix4x4& newTransform = *(++_impl->_currentTransform);

  newTransform = (oldTransform * tr).matrix();
  _impl->_modelTransformData.update(_impl->_d3dContext.Get(), newTransform.transpose().data());
  }

void D3DRenderer::popTransform( )
  {
  _impl->_currentTransform--;
  xAssert(_impl->_currentTransform >= _impl->_transformStack);
  }

void D3DRenderer::setClearColour(const Colour &col)
  {
  _impl->_clearColour = col;
  }

void D3DRenderer::clear(int clear)
  {
  _impl->_renderTarget.clear(
        _impl->_d3dContext.Get(),
        (clear&Renderer::ClearColour) != 0,
        (clear&Renderer::ClearDepth) != 0,
        _impl->_clearColour.data(),
        1.0f,
        0);
  }

bool D3DRenderer::createShader(Shader *s, ShaderVertexComponent *v, ShaderFragmentComponent *f)
  {
  XD3DFragmentShaderImpl *frag = f->data<XD3DFragmentShaderImpl>();
  XD3DVertexShaderImpl *vert = v->data<XD3DVertexShaderImpl>();

  XD3DSurfaceShaderImpl* shd = s->data<XD3DSurfaceShaderImpl>();
  new(shd) XD3DSurfaceShaderImpl();

  shd->_pixelShader = frag->_pixelShader;
  shd->_vertexShader = vert->_vertexShader;

  return shd->_pixelShader && shd->_vertexShader;
  }

bool D3DRenderer::createVertexShaderComponent(ShaderVertexComponent *v,
                                               const char *s,
                                               xsize l,
                                               const ShaderVertexLayoutDescription *vertexDescriptions,
                                               xsize vertexItemCount,
                                               ShaderVertexLayout *layout)
  {
  XD3DVertexShaderImpl *vert = v->data<XD3DVertexShaderImpl>();
  new(vert) XD3DVertexShaderImpl();

  if(!vert->create(_impl->_d3dDevice.Get(), s, l))
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
    XD3DShaderInputLayout *lay = layout->data<XD3DShaderInputLayout>();
    new(lay) XD3DShaderInputLayout();
    if(failedCheck(
      _impl->_d3dDevice->CreateInputLayout(
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

bool D3DRenderer::createFragmentShaderComponent(ShaderFragmentComponent *f, const char *s, xsize l)
  {
  XD3DFragmentShaderImpl *frag = f->data<XD3DFragmentShaderImpl>();
  new(frag) XD3DFragmentShaderImpl();
  return frag->create(_impl->_d3dDevice.Get(), s, l);
  }

bool D3DRenderer::createGeometry(
    Geometry *g,
    const void *data,
    xsize elementSize,
    xsize elementCount)
  {
  XD3DVertexBufferImpl *geo = g->data<XD3DVertexBufferImpl>();
  new(geo) XD3DVertexBufferImpl();

  xsize dataSize = elementSize * elementCount;

  bool result = geo->create(_impl->_d3dDevice.Get(), data, dataSize, D3D11_BIND_VERTEX_BUFFER);

  geo->elementSize = elementSize;
  geo->elementCount = elementCount;

  return result;
  }


bool D3DRenderer::createIndexGeometry(
    IndexGeometry *g,
    int type,
    const void *index,
    xsize indexCount)
  {
  XD3DIndexBufferImpl *geo = g->data<XD3DIndexBufferImpl>();
  new(geo) XD3DIndexBufferImpl();

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
  bool result = geo->create(_impl->_d3dDevice.Get(), index, dataSize, D3D11_BIND_INDEX_BUFFER);

  geo->count = indexCount;

  return result;
  }

void D3DRenderer::debugRenderLocator(DebugLocatorMode)
  {
  }

void D3DRenderer::drawTriangles(const Geometry *vert)
  {
  const XD3DVertexBufferImpl *geo = vert->data<XD3DVertexBufferImpl>();

  UINT stride = geo->elementSize;
  UINT offset = 0;
  _impl->_d3dContext->IASetVertexBuffers(
    0,
    1,
    geo->buffer.GetAddressOf(),
    &stride,
    &offset
    );

  _impl->_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  _impl->_d3dContext->Draw(
    geo->elementCount,
    0
    );
  }

void D3DRenderer::drawTriangles(const IndexGeometry *indices, const Geometry *vert)
  {
  const XD3DVertexBufferImpl *geo = vert->data<XD3DVertexBufferImpl>();
  const XD3DIndexBufferImpl *idx = indices->data<XD3DIndexBufferImpl>();

  UINT stride = geo->elementSize;
  UINT offset = 0;
  _impl->_d3dContext->IASetVertexBuffers(
    0,
    1,
    geo->buffer.GetAddressOf(),
    &stride,
    &offset
    );

  _impl->_d3dContext->IASetIndexBuffer(
    idx->buffer.Get(),
    idx->format,
    0
    );

  _impl->_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  _impl->_d3dContext->DrawIndexed(
    idx->count,
    0,
    0
    );
  }

void D3DRenderer::destroyShader(Shader* s)
  {
  XD3DSurfaceShaderImpl* shd = s->data<XD3DSurfaceShaderImpl>();
  shd->~XD3DSurfaceShaderImpl();
  }

void D3DRenderer::destroyShaderVertexLayout(ShaderVertexLayout *d)
  {
  XD3DShaderInputLayout* lay = d->data<XD3DShaderInputLayout>();
  lay->~XD3DShaderInputLayout();
  }

void D3DRenderer::destroyVertexShaderComponent(ShaderVertexComponent* s)
  {
  XD3DVertexShaderImpl *vert = s->data<XD3DVertexShaderImpl>();
  vert->~XD3DVertexShaderImpl();
  }

void D3DRenderer::destroyFragmentShaderComponent(ShaderFragmentComponent* s)
  {
  XD3DFragmentShaderImpl *frag = s->data<XD3DFragmentShaderImpl>();
  frag->~XD3DFragmentShaderImpl();
  }

void D3DRenderer::destroyGeometry( Geometry *g )
  {
  XD3DVertexBufferImpl *geo = g->data<XD3DVertexBufferImpl>();
  geo->~XD3DVertexBufferImpl();
  }

void D3DRenderer::destroyIndexGeometry( IndexGeometry *g )
  {
  XD3DIndexBufferImpl *geo = g->data<XD3DIndexBufferImpl>();
  geo->~XD3DIndexBufferImpl();
  }

void D3DRenderer::setViewTransform(const Transform &v)
  {
  _impl->_worldTransformData.data.view = v.matrix().transpose();
  _impl->_updateWorldTransformData = true;
  }

void D3DRenderer::setProjectionTransform(const ComplexTransform &p)
  {
  _impl->_worldTransformData.data.projection = p.matrix().transpose();
  _impl->_updateWorldTransformData = true;
  }

void D3DRenderer::setShader(const Shader *s, const ShaderVertexLayout *layout)
  {
  const XD3DSurfaceShaderImpl* shd = s->data<XD3DSurfaceShaderImpl>();
  shd->bind(_impl->_d3dContext.Get());

  ID3D11Buffer *buffers[] =
  {
    _impl->_worldTransformData.buffer.Get(),
    _impl->_modelTransformData.buffer.Get()
  };

  _impl->_d3dContext->VSSetConstantBuffers(
    0,
    X_ARRAY_COUNT(buffers),
    buffers
    );


  const XD3DShaderInputLayout* lay = layout->data<XD3DShaderInputLayout>();
  _impl->_d3dContext->IASetInputLayout(lay->_inputLayout.Get());
  }

void D3DRenderer::setFramebuffer( const Framebuffer * )
  {
  }

void D3DRenderer::enableRenderFlag( RenderFlags )
  {
  }

void D3DRenderer::disableRenderFlag( RenderFlags )
  {
  }

}
