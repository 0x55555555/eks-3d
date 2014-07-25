#pragma once
#include "Math/XMathHelpers.h"


namespace Eks
{

namespace Demo
{

class ExampleBase
  {
public:
  virtual void initialise(Renderer* r) = 0;
  virtual void resize(Renderer*, xuint32 width, xuint32 height) = 0;
  virtual void render(Renderer* r, Eks::FrameBuffer *buffer) = 0;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
