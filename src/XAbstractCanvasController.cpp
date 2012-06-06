#include "XAbstractCanvasController.h"

XAbstractCanvasController::XAbstractCanvasController(XAbstractCanvas *canvas) : _canvas(canvas)
  {
  }


XAbstractCanvasController::UsedFlags XAbstractCanvasController::triggerMouseEvent(MouseEventType type,
                       QPoint point,
                       Qt::MouseButton triggerButton,
                       Qt::MouseButtons buttonsDown,
                       Qt::KeyboardModifiers modifiers)
  {
  MouseEvent e;
  e.type = type;
  e.point = point;
  e.lastPoint = lastKnownMousePosition();
  e.triggerButton = triggerButton;
  e.buttonsDown = buttonsDown;
  e.modifiers = modifiers;

  UsedFlags ret = mouseEvent(e);

  _lastKnownMousePosition = point;
  return ret;
  }



XAbstractCanvasController::UsedFlags XAbstractCanvasController::triggerWheelEvent(int delta,
                       Qt::Orientation orientation,
                       QPoint point,
                       Qt::MouseButtons buttonsDown,
                       Qt::KeyboardModifiers modifiers)
  {
  WheelEvent w;
  w.delta = delta;
  w.orientation = orientation;
  w.point = point;
  w.buttonsDown = buttonsDown;
  w.modifiers = modifiers;

  UsedFlags ret = wheelEvent(w);

  _lastKnownMousePosition = point;
  return ret;
  }
