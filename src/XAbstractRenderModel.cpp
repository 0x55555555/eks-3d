#include "XAbstractRenderModel.h"
#include "XAbstractCanvas.h"

XAbstractRenderModel::Iterator::~Iterator()
  {
  }

XAbstractRenderModel::~XAbstractRenderModel()
  {
  foreach(XAbstractCanvas *canvas, _canvases)
    {
    canvas->setModel(0);
    }
  }

void XAbstractRenderModel::update(UpdateMode m) const
  {
  foreach(XAbstractCanvas *canvas, _canvases)
    {
    canvas->update(m);
    }
  }
