#ifndef XDEPTHSTENCILSTATE_H
#define XDEPTHSTENCILSTATE_H

#include "X3DGlobal.h"
#include "Utilities/XPrivateImpl.h"

namespace Eks
{

class Renderer;

class EKS3D_EXPORT DepthStencilState : public PrivateImpl<sizeof(xuint32) * 7>
  {
public:
  enum DrawMask
    {
    ColourR = 1,
    ColourG = 2,
    ColourB = 4,
    ColourA = 8,
    Depth = 16,
    Stencil = 32,

    MaskCount,

    Colour = ColourR | ColourG | ColourB | ColourA,
    MaskAll = Colour | Depth | Stencil,
    };

  enum Tests
    {
    DepthTest = 1,
    StencilTest = 2,

    TestCount,

    TestNone = 0,
    };

  enum Function
    {
    Never,
    Less,
    Equal,
    LEqual,
    Greater,
    NotEqual,
    GEqual,
    Always,

    FunctionCount,
    };

  DepthStencilState(Renderer *r=0,
    xuint32 writeMask = MaskAll,
    xuint32 tests = TestNone,
    Function depthTest = Less,
    Function stencilTest = Always,
    xint32 stencilRef = 0x0,
    xuint32 stencilMask = 0xFFFFFFFF,
    float depthNear = 0.0f,
    float depthFar = 1.0f);
  ~DepthStencilState();

  static bool delayedCreate(
    DepthStencilState &ths,
    Renderer *r,
    xuint32 writeMask,
    xuint32 tests,
    Function depthTest = Less,
    Function stencilTest = Always,
    xint32 stencilRef = 0x0,
    xuint32 stencilMask = 0xFFFFFFFF,
    float depthNear = 0.0f,
    float depthFar = 1.0f);

private:
  Renderer *_renderer;
  };

}

#endif // XDEPTHSTENCILSTATE_H
