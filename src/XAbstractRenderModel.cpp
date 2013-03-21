#include "XAbstractRenderModel.h"
#include "XAbstractCanvas.h"

namespace Eks
{

AbstractRenderModel::Iterator::~Iterator()
  {
  }

AbstractRenderModel::~AbstractRenderModel()
  {
  //xForeach(AbstractCanvas *canvas, _canvases)
    {
    //canvas->setModel(0);
    }
  }

void AbstractRenderModel::update(UpdateMode) const
  {
  //xForeach(AbstractCanvas *canvas, _canvases)
    {
    //canvas->update(m);
    }
  }
}

