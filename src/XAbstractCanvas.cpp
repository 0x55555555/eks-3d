#include "XAbstractCanvas.h"
#include "XAbstractRenderModel.h"
#include "XAbstractDelegate.h"
#include "XAbstractCanvasController.h"

namespace Eks
{

AbstractCanvas::AbstractCanvas(AbstractCanvasController *c)
    : _controller(c)
  {
  }

AbstractCanvas::~AbstractCanvas()
  {
  }
}
