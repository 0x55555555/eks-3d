#pragma once
#include "Math/XMathHelpers.h"


namespace Eks
{

namespace Demo
{

class ExampleBase
  {
public:
  virtual void intialise(Renderer* r) = 0;
  virtual void resize(Renderer*, xuint32 width, xuint32 height) = 0;
  virtual void render(Renderer* r) = 0;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}
