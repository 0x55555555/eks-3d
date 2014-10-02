#ifndef XABSTRACTSHADER_H
#define XABSTRACTSHADER_H

#include "X3DGlobal.h"
#include "Utilities/XPrivateImpl.h"

namespace Eks
{

class ParseErrorInterface;
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
    BiNormal,

    SemanticCount,

    InvalidSemantic
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

class EKS3D_EXPORT ShaderComponent : public PrivateImpl<sizeof(void*) * 2>
  {
public:
  enum ShaderType
    {
    Vertex,
    TesselationControl,
    TesselationEvaluator,
    Fragment,
    Geometry,

    ShaderComponentCount
    };

  ShaderComponent(Renderer *r = nullptr,
                  ShaderType t = Vertex,
                  const char *source = nullptr,
                  xsize length = 0,
                  ParseErrorInterface *errors = nullptr,
                  const void *extra = nullptr);
  ~ShaderComponent();

  static bool delayedCreate(ShaderComponent &ths,
                            Renderer *r,
                            ShaderType t,
                            const char *source,
                            xsize length,
                            ParseErrorInterface *errors = nullptr,
                            const void *extraData = nullptr);

protected:
  X_DISABLE_COPY(ShaderComponent);

  Renderer *_renderer;
  };

class EKS3D_EXPORT ShaderVertexComponent : public ShaderComponent
  {
public:
  typedef ShaderVertexLayout VertexLayout;

  struct ExtraCreateData
    {
    const ShaderVertexLayoutDescription *vertexDescriptions;
    xsize vertexItemCount;
    ShaderVertexLayout *layout;
    };

  ShaderVertexComponent(Renderer *r=nullptr,
                            const char *source=nullptr,
                            xsize length=0,
                            const VertexLayout::Description *vertexDescriptions=nullptr,
                            xsize vertexItemCount=0,
                            VertexLayout *layout=nullptr,
                            ParseErrorInterface *ifc = nullptr);

  static bool delayedCreate(ShaderVertexComponent &ths,
                            Renderer *r,
                            const char *source,
                            xsize length,
                            const VertexLayout::Description *vertexDescription,
                            xsize vertexItemCount,
                            VertexLayout *layout,
                            ParseErrorInterface *ifc = nullptr);


private:
  X_DISABLE_COPY(ShaderVertexComponent);
  };

class EKS3D_EXPORT Shader : public PrivateImpl<sizeof(void*)*6>
  {
public:
  typedef ShaderConstantData ConstantData;

  Shader(Renderer *r=0,
         ShaderComponent **v=0,
         xsize componentCount=0,
         const char **outputs=0,
         xsize outputCount=0,
         ParseErrorInterface *errors = nullptr);
  ~Shader();

  static bool delayedCreate(Shader &ths,
              Renderer *r,
              ShaderComponent **v,
              xsize componentCount,
              const char **outputs,
              xsize outputCount,
              ParseErrorInterface *errors = nullptr);


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
