#ifndef XABSTRACTSHADER_H
#define XABSTRACTSHADER_H

#include "X3DGlobal.h"
#include "XPrivateImpl"

namespace Eks
{

class Shader;
class Renderer;


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
    OffsetPackTight = X_SIZE_SENTINEL
    };

  enum Semantic
    {
    Position,
    Colour,
    TextureCoordinate,
    Normal,

    SemanticCount
    };

  ShaderVertexLayoutDescription(Semantic s, Format fmt, xsize off=OffsetPackTight, Slot sl=Slot())
    : semantic(s), format(fmt), offset(off), slot(sl)
    {
    }

  Semantic semantic;
  Format format;
  xsize offset;
  Slot slot;
  };

class ShaderVertexLayout : public PrivateImpl<sizeof(void*)>
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

class EKS3D_EXPORT ShaderVertexComponent : public PrivateImpl<sizeof(void*)>
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

class EKS3D_EXPORT ShaderFragmentComponent : public PrivateImpl<sizeof(void*)>
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

class EKS3D_EXPORT Shader : public PrivateImpl<sizeof(void*)*2>
  {
public:
  Shader(Renderer *r=0,
          ShaderVertexComponent *v=0,
          ShaderFragmentComponent *f=0);
  ~Shader();

  static bool delayedCreate(Shader &ths,
              Renderer *r,
              ShaderVertexComponent *v,
              ShaderFragmentComponent *f);

private:
  X_DISABLE_COPY(Shader);

  Renderer *_renderer;
  };

}

#endif // XABSTRACTSHADER_H
