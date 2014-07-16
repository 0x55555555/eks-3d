#include "QWidget"
#include "QTimer"
#include "XRenderer.h"
#include "XAbstractCanvas.h"
#include "ExampleBase.h"

class ExampleViewer : public QWidget
  {
  Q_OBJECT
public:
  ExampleViewer();

  constexpr static const float fps = 60.0f;

private slots:
  void initialise3D(Eks::Renderer *r);
  void paint3D(Eks::Renderer *r, Eks::FrameBuffer *buffer);
  void resize3D(Eks::Renderer *r, xuint32 w, xuint32 h);

private:
  Eks::AbstractCanvas *_canvas;
  QWidget *_viewport;
  QTimer _timer;

  std::vector<std::unique_ptr<Eks::Demo::ExampleBase>> _examples;
  Eks::Demo::ExampleBase* _activeExample;
  };
