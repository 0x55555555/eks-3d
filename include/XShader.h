#ifndef XABSTRACTSHADER_H
#define XABSTRACTSHADER_H

#include "X3DGlobal.h"
#include "Utilities/XPrivateImpl.h"

namespace Eks
{

class Shader;
class Renderer;
class Resource;

class ShaderVertexLayoutDescription
  {
public:
  enum Format
    {
    FormatFloat1,
    FormatFloat2,
    FormatFloat3,
    FormatFloat4,

    FormatCount
    };

  struct Slot
    {
  public:
    enum Type
      {
      PerVertex,
      PerInstance
      };
    Slot() : index(0), type(PerVertex), instanceDataStepRate(0)
      {
      }

    xuint8 index;
    Type type;
    xsize instanceDataStepRate;
    };

  enum OffsetPack
    {
    OffsetPackTight = std::numeric_limits<xuint32>::max()
    };

  enum Semantic
    {
    Position,
    Colour,
    TextureCoordinate,
    Normal,

    SemanticCount
    };

  ShaderVertexLayoutDescription(Semantic s=SemanticCount, Format fmt=FormatCount, xsize off=OffsetPackTight, Slot sl=Slot())
    : semantic(s), format(fmt), offset(off), slot(sl)
    {
    }

  Semantic semantic;
  Format format;
  xsize offset;
  Slot slot;
  };

class EKS3D_EXPORT ShaderVertexLayout : public PrivateImpl<sizeof(void*) * 6>
  {
public:
  typedef ShaderVertexLayoutDescription Description;
  ShaderVertexLayout() : _renderer(0) { }
  ~ShaderVertexLayout();

private:
  X_DISABLE_COPY(ShaderVertexLayout);
  friend class ShaderVertexComponent;

  Renderer *_renderer;
  };

class ShaderConstantDataDescription
  {
public:
  enum Type
    {
    Float,
    Float3,
    Float4,
    Matrix4x4,

    TypeCount
    };
  const char* name;
  Type type;
  };

class EKS3D_EXPORT ShaderConstantData : public PrivateImpl<sizeof(void*) * 9>
  {
public:
  typedef ShaderConstantDataDescription Description;

  ShaderConstantData(Renderer *r=0,
                     Description *desc = 0,
                     xsize descCount = 0,
                     const void *data=0);
  ~ShaderConstantData();

  static bool delayedCreate(ShaderConstantData &ths,
                            Renderer *r,
                            Description *desc = 0,
                            xsize descCount = 0,
                            const void *data = 0);

  void update(void *data);

private:
  X_DISABLE_COPY(ShaderConstantData);

  Renderer *_renderer;
  };

class EKS3D_EXPORT ShaderVertexComponent : public PrivateImpl<sizeof(void*) * 2>
  {
public:
  typedef ShaderVertexLayout VertexLayout;

  ShaderVertexComponent(Renderer *r=0,
                         const char *source=0,
                         xsize length=0,
                         const VertexLayout::Description *vertexDescriptions=0,
                         xsize vertexItemCount=0,
                         VertexLayout *layout=0);
  ~ShaderVertexComponent();

  static bool delayedCreate(ShaderVertexComponent &ths,
                            Renderer *r,
                            const char *source,
                            xsize length,
                            const VertexLayout::Description *vertexDescription=0,
                            xsize vertexItemCount=0,
                            VertexLayout *layout=0);


private:
  X_DISABLE_COPY(ShaderVertexComponent);

  Renderer *_renderer;
  };

class EKS3D_EXPORT ShaderFragmentComponent : public PrivateImpl<sizeof(void*) * 2>
  {
public:
  ShaderFragmentComponent(Renderer *r=0,
                          const char *source=0,
                          xsize length=0);
  ~ShaderFragmentComponent();

  static bool delayedCreate(ShaderFragmentComponent &ths,
                            Renderer *r,
                            const char *source,
                            xsize length);

private:
  X_DISABLE_COPY(ShaderFragmentComponent);

  Renderer *_renderer;
  };

class EKS3D_EXPORT Shader : public PrivateImpl<sizeof(void*)*6>
  {
public:
  typedef ShaderConstantData ConstantData;

  Shader(Renderer *r=0,
         ShaderVertexComponent *v=0,
         ShaderFragmentComponent *f=0,
         const char **outputs=0,
         xsize outputCount=0);
  ~Shader();

  static bool delayedCreate(Shader &ths,
              Renderer *r,
              ShaderVertexComponent *v,
              ShaderFragmentComponent *f,
              const char **outputs,
              xsize outputCount);


  void setShaderConstantData(xsize first, const ConstantData *data);
  void setShaderConstantDatas(xsize first, xsize num, const ConstantData *const* data);

  void setShaderResource(xsize first, const Resource *data);
  void setShaderResources(xsize first, xsize num, const Resource *const* data);

private:
  X_DISABLE_COPY(Shader);

  Renderer *_renderer;
  };

}

#include "XRenderer.h"

namespace Eks
{

inline void ShaderConstantData::update(void *data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderConstantData(_renderer, this, data);
  }

inline void Shader::setShaderConstantData(xsize first, const ConstantData *data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderConstantBuffer(_renderer, this, first, 1, &data);
  }

inline void Shader::setShaderConstantDatas(xsize first, xsize num, const ConstantData *const* data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderConstantBuffer(_renderer, this, first, num, data);
  }

inline void Shader::setShaderResource(xsize first, const Resource *data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderResource(_renderer, this, first, 1, &data);
  }

inline void Shader::setShaderResources(xsize first, xsize num, const Resource *const* data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderResource(_renderer, this, first, num, data);
  }

}

#endif // XABSTRACTSHADER_H
