#include "XAbstractRenderModel.h"
#include "XAbstractCanvas.h"

XAbstractRenderModel::Iterator::~Iterator()
  {
  }

XAbstractRenderModel::~XAbstractRenderModel()
  {
  Q_FOREACH(XAbstractCanvas *canvas, _canvases)
    {
    canvas->setModel(0);
    }
  }

void XAbstractRenderModel::update(UpdateMode m) const
  {
  Q_FOREACH(XAbstractCanvas *canvas, _canvases)
    {
    canvas->update(m);
    }
  }
