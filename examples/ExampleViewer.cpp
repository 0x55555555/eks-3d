#include "QtWidgets/QApplication"
#include "X3DCanvas.h"
#include "XCore.h"

class ExampleViewer : public QWidget
  {
public:
  ExampleViewer()
    {
    _viewport = Eks::Canvas3D::createBest(this, &_canvas);
    }

private:
  Eks::AbstractCanvas *_canvas;
  QWidget *_viewport;
  };

int main(int argc, char** argv)
  {
  QApplication app(argc, argv);

  Eks::Core core;

  ExampleViewer viewer;
  viewer.show();

  return app.exec();
  }
