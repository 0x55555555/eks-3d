#ifndef XABSTRACTSHADER_H
#define XABSTRACTSHADER_H

#include "X3DGlobal.h"
#include "XPrivateImpl"

class XShader;
class XRenderer;


class XShaderVertexLayoutDescription
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

  XShaderVertexLayoutDescription(Semantic s, Format fmt, xsize off=OffsetPackTight, Slot sl=Slot())
    : semantic(s), format(fmt), offset(off), slot(sl)
    {
    }

  Semantic semantic;
  Format format;
  xsize offset;
  Slot slot;
  };

class XShaderVertexLayout : public XPrivateImpl<sizeof(void*)>
  {
public:
  typedef XShaderVertexLayoutDescription Description;
  XShaderVertexLayout() : _renderer(0) { }
  ~XShaderVertexLayout();

private:
  X_DISABLE_COPY(XShaderVertexLayout);
  friend class XShaderVertexComponent;

  XRenderer *_renderer;
  };

class EKS3D_EXPORT XShaderVertexComponent : public XPrivateImpl<sizeof(void*)>
  {
public:
  typedef XShaderVertexLayout VertexLayout;

  XShaderVertexComponent(XRenderer *r=0,
                         const char *source=0,
                         xsize length=0,
                         const VertexLayout::Description *vertexDescriptions=0,
                         xsize vertexItemCount=0,
                         VertexLayout *layout=0);
  ~XShaderVertexComponent();

  static bool delayedCreate(XShaderVertexComponent &ths,
                            XRenderer *r,
                            const char *source,
                            xsize length,
                            const VertexLayout::Description *vertexDescription=0,
                            xsize vertexItemCount=0,
                            VertexLayout *layout=0);


private:
  X_DISABLE_COPY(XShaderVertexComponent);

  XRenderer *_renderer;
  };

class EKS3D_EXPORT XShaderFragmentComponent : public XPrivateImpl<sizeof(void*)>
  {
public:
  XShaderFragmentComponent(XRenderer *r=0,
                           const char *source=0,
                           xsize length=0);
  ~XShaderFragmentComponent();

  static bool delayedCreate(XShaderFragmentComponent &ths,
                            XRenderer *r,
                            const char *source,
                            xsize length);

private:
  X_DISABLE_COPY(XShaderFragmentComponent);

  XRenderer *_renderer;
  };

class EKS3D_EXPORT XShader : public XPrivateImpl<sizeof(void*)*2>
  {
public:
  XShader(XRenderer *r=0,
          XShaderVertexComponent *v=0,
          XShaderFragmentComponent *f=0);
  ~XShader();

  static bool delayedCreate(XShader &ths,
              XRenderer *r,
              XShaderVertexComponent *v,
              XShaderFragmentComponent *f);

private:
  X_DISABLE_COPY(XShader);

  XRenderer *_renderer;
  };

#endif // XABSTRACTSHADER_H
