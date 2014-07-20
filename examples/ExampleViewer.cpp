#include "QtWidgets/QApplication"
#include "QtWidgets/QVBoxLayout"
#include "ExampleViewer.h"
#include "X3DCanvas.h"
#include "XFramebuffer.h"
#include "XCore.h"

#include "simple/simple.h"
#include "geometry/geometry.h"
#include "textures/textures.h"
#include "normalmap/normalmap.h"
#include "displacementmap/displacementmap.h"

ExampleViewer::ExampleViewer()
  {
  resize(800, 600);
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  _viewport = Eks::Canvas3D::createBest(this, &_canvas);
  layout->addWidget(_viewport);

  _timer.start(1000/fps);

  connect(_viewport, SIGNAL(initialise3D(Eks::Renderer*)), this, SLOT(initialise3D(Eks::Renderer*)));
  connect(_viewport, SIGNAL(paint3D(Eks::Renderer*,Eks::FrameBuffer*)), this, SLOT(paint3D(Eks::Renderer*,Eks::FrameBuffer*)));
  connect(_viewport, SIGNAL(resize3D(Eks::Renderer*,xuint32,xuint32)), this, SLOT(resize3D(Eks::Renderer*,xuint32,xuint32)));

  connect(&_timer, SIGNAL(timeout()), _viewport, SLOT(update()));

  _examples.emplace_back(new Eks::Demo::SimpleExample());
  _examples.emplace_back(new Eks::Demo::GeometryExample());
  _examples.emplace_back(new Eks::Demo::TextureExample());
  _examples.emplace_back(new Eks::Demo::NormalMapExample());
  _examples.emplace_back(new Eks::Demo::DisplacementMapExample());
  _activeExample = _examples.back().get();
  }

void ExampleViewer::initialise3D(Eks::Renderer *r)
  {
  _activeExample->intialise(r);
  }

void ExampleViewer::paint3D(Eks::Renderer *r, Eks::FrameBuffer *buffer)
  {
  Eks::FrameBufferRenderFrame fr(r, buffer);
  _activeExample->render(r);
  }

void ExampleViewer::resize3D(Eks::Renderer *r, xuint32 w, xuint32 h)
  {
  _activeExample->resize(r, w, h);
  }

int main(int argc, char** argv)
  {
  QApplication app(argc, argv);

  Eks::Core core;

  ExampleViewer viewer;
  viewer.show();

  return app.exec();
  }
