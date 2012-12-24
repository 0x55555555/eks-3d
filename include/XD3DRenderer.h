#ifndef XD3DRENDERER_H
#define XD3DRENDERER_H

#include "XGlobal"
#include "XRenderer.h"

struct IUnknown;
struct ID3D11Device1;
struct ID3D11DeviceContext1;

namespace Eks
{

class D3DRenderer;
class D3DRendererImpl;

class D3DRendererRenderFrame
  {
public:
  D3DRendererRenderFrame(D3DRenderer &r, bool *deviceLost);
  ~D3DRendererRenderFrame();

private:
  D3DRenderer &_renderer;
  bool *_deviceLost;
  };

class EKS3D_EXPORT D3DRenderer : public Renderer
  {
public:
  D3DRenderer(IUnknown *window);
  ~D3DRenderer();

  enum
    {
    UserVSContantBufferOffset = 2,
    UserPSContantBufferOffset = 0
    };

  typedef D3DRendererRenderFrame RenderFrame;

  bool resize(xuint32 w, xuint32 h, Rotation rotation);

  ID3D11Device1 *getD3DDevice();
  ID3D11DeviceContext1 *getD3DContext();

  void beginFrame();
  void endFrame(bool *deviceLost);

  void pushTransform( const Transform & ) X_OVERRIDE;
  void popTransform( ) X_OVERRIDE;

  void setClearColour(const Colour &col) X_OVERRIDE;
  void clear(int=ClearColour|ClearDepth) X_OVERRIDE;

  // creation accessors for abstract types
  bool createShader(
    Shader *s,
    ShaderVertexComponent *v,
    ShaderFragmentComponent *f) X_OVERRIDE;

  bool createVertexShaderComponent(
    ShaderVertexComponent *v,
    const char *s,
    xsize l,
    const ShaderVertexLayoutDescription *vertexDescriptions,
    xsize vertexItemCount,
    ShaderVertexLayout *layout) X_OVERRIDE;

  bool createFragmentShaderComponent(
    ShaderFragmentComponent *f,
    const char *s,
    xsize l) X_OVERRIDE;

  bool createGeometry(
    Geometry *g,
    const void *data,
    xsize elementSize,
    xsize elementCount) X_OVERRIDE;

  bool createIndexGeometry(
    IndexGeometry *g,
    int type,
    const void *index,
    xsize indexCount) X_OVERRIDE;

  bool createRasteriserState(
      RasteriserState *s,
      RasteriserState::CullMode cull) X_OVERRIDE;

  bool createShaderConstantData(
      ShaderConstantData *,
      xsize size,
      void *data) X_OVERRIDE;

  void updateShaderConstantData(ShaderConstantData *, void *data) X_OVERRIDE;

  void debugRenderLocator(DebugLocatorMode) X_OVERRIDE;

  // destroy abstract types
  void destroyShader(Shader* s) X_OVERRIDE;
  void destroyShaderVertexLayout(ShaderVertexLayout *d) X_OVERRIDE;
  void destroyVertexShaderComponent(ShaderVertexComponent* s) X_OVERRIDE;
  void destroyFragmentShaderComponent(ShaderFragmentComponent* s) X_OVERRIDE;
  void destroyGeometry( Geometry * ) X_OVERRIDE;
  void destroyIndexGeometry( IndexGeometry * ) X_OVERRIDE;
  void destroyRasteriserState(RasteriserState *) X_OVERRIDE;
  void destroyShaderConstantData(ShaderConstantData *) X_OVERRIDE;

  void setViewTransform( const Transform & ) X_OVERRIDE;
  void setProjectionTransform( const ComplexTransform & ) X_OVERRIDE;

  void setFragmentShaderConstantBuffer(
    Shader *shader,
    xsize index,
    const ShaderConstantData *data) X_OVERRIDE;

  void setVertexShaderConstantBuffer(
    Shader *shader,
    xsize index,
    const ShaderConstantData *data) X_OVERRIDE;

  // set the current shader
  void setShader( const Shader *, const ShaderVertexLayout *layout ) X_OVERRIDE;

  void setRasteriserState(const RasteriserState *state) X_OVERRIDE;

  // draw the given geometry
  void drawTriangles(const IndexGeometry *indices, const Geometry *vert) X_OVERRIDE;
  void drawTriangles(const Geometry *vert) X_OVERRIDE;

  // bind the given framebuffer for drawing
  void setFramebuffer( const Framebuffer * ) X_OVERRIDE;

private:
  D3DRendererImpl *_impl;
  };

inline D3DRendererRenderFrame::D3DRendererRenderFrame(D3DRenderer &r, bool *deviceLost)
    : _renderer(r), _deviceLost(deviceLost)
  {
  r.beginFrame();
  }

inline D3DRendererRenderFrame::~D3DRendererRenderFrame()
  {
  _renderer.endFrame(_deviceLost);
  }

}

#endif // XD3DRENDERER_H
