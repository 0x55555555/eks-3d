#ifndef XBLENDSTATE_H
#define XBLENDSTATE_H

#include "X3DGlobal.h"
#include "Utilities/XPrivateImpl.h"
#include "Math/XColour.h"

namespace Eks
{

class Renderer;

class EKS3D_EXPORT BlendState : public PrivateImpl<sizeof(xuint32) * 12>
  {
public:
  enum BlendMode
    {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,

    ModeCount,
    };

  enum BlendParameter
    {
    Zero,
    One,
    SrcColour,
    OneMinusSrcColour,
    DstColour,
    OneMinusDstColour,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColour,
    OneMinusConstantColour,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
    Src1Colour,
    OneMinusSrc1Colour,
    Src1Alpha,
    OneMinusSrc1Alpha,

    ParameterCount,
    };

  BlendState(
    Renderer * = nullptr,
    bool enable = false,
    BlendMode = Add,
    BlendParameter = SrcAlpha,
    BlendParameter = OneMinusSrcAlpha,
    BlendMode = Add,
    BlendParameter = SrcAlpha,
    BlendParameter = OneMinusSrcAlpha,
    const Eks::Colour &col = Eks::Colour::Zero());
  ~BlendState();

  static bool delayedCreate(
    BlendState &ths,
    Renderer *r,
    bool enable,
    BlendMode = Add,
    BlendParameter = SrcAlpha,
    BlendParameter = OneMinusSrcAlpha,
    BlendMode = Add,
    BlendParameter = SrcAlpha,
    BlendParameter = OneMinusSrcAlpha,
    const Eks::Colour &col = Eks::Colour::Zero());

private:
  Renderer *_renderer;
  };

}

#endif // XBLENDSTATE_H
