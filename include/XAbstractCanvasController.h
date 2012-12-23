#ifndef XABSTRACTCANVASCONTROLLER_H
#define XABSTRACTCANVASCONTROLLER_H

#include "X3DGlobal.h"
#include "XProperty"
#include "QEvent"
#include "QPoint"
#include "XFlags"

#define X_IMPLEMENT_MOUSEHANDLER(function, type, update) \
virtual void function(QMouseEvent *event) { \
  if(controller()) { \
    Eks::AbstractCanvasController::UsedFlags result = controller()->triggerMouseEvent( \
                                                  Eks::AbstractCanvasController::type, \
                                                  event->pos(), \
                                                  event->button(), \
                                                  event->buttons(), \
                                                  event->modifiers()); \
    if((result.hasFlag(Eks::AbstractCanvasController::Used))) { event->accept(); } \
    if((result.hasFlag(Eks::AbstractCanvasController::NeedsUpdate))) { update(Eks::AbstractRenderModel::RenderChange); } \
    return; } \
  event->ignore(); }

#define X_CANVAS_GENERAL_MOUSEHANDLERS() \
  X_IMPLEMENT_MOUSEHANDLER(mouseDoubleClickEvent, DoubleClick, update) \
  X_IMPLEMENT_MOUSEHANDLER(mouseMoveEvent, Move, update) \
  X_IMPLEMENT_MOUSEHANDLER(mousePressEvent, Press, update) \
  X_IMPLEMENT_MOUSEHANDLER(mouseReleaseEvent, Release, update) \
  virtual void wheelEvent(QWheelEvent *event) { \
    if(controller()) { \
      Eks::AbstractCanvasController::UsedFlags result = controller()->triggerWheelEvent( \
                                                    event->delta(), \
                                                    event->orientation(), \
                                                    event->pos(), \
                                                    event->buttons(), \
                                                    event->modifiers()); \
      if((result.hasFlag(Eks::AbstractCanvasController::Used))) { event->accept(); } \
      if((result.hasFlag(Eks::AbstractCanvasController::NeedsUpdate))) { update(Eks::AbstractRenderModel::RenderChange); } \
      return; } \
    event->ignore(); }

namespace Eks
{

class AbstractCanvas;

class EKS3D_EXPORT AbstractCanvasController
  {
XProperties:
  XROProperty(AbstractCanvas *, canvas);
  XROProperty(QPoint, lastKnownMousePosition);

public:
  enum MouseEventType
    {
    DoubleClick,
    Move,
    Press,
    Release
    };

  AbstractCanvasController(AbstractCanvas *canvas);

  enum Result
    {
    NotUsed = 0,
    Used = 1,
    NeedsUpdate = 2
    };
  typedef XFlags<Result, xint32> UsedFlags;

  virtual xuint32 maxNumberOfPasses(xuint32) const { return 0; }
  virtual void paint(xuint32) const { }

  UsedFlags triggerMouseEvent(MouseEventType type,
                              QPoint point,
                              Qt::MouseButton triggerButton,
                              Qt::MouseButtons buttonsDown,
                              Qt::KeyboardModifiers modifiers);

  UsedFlags triggerWheelEvent(int delta,
                              Qt::Orientation orientation,
                              QPoint point,
                              Qt::MouseButtons buttonsDown,
                              Qt::KeyboardModifiers modifiers);

protected:
  struct MouseEvent
    {
    MouseEventType type;
    QPoint point;
    QPoint lastPoint;
    Qt::MouseButton triggerButton;
    Qt::MouseButtons buttonsDown;
    Qt::KeyboardModifiers modifiers;
    };

  struct WheelEvent
    {
    int delta;
    Qt::Orientation orientation;
    QPoint point;
    Qt::MouseButtons buttonsDown;
    Qt::KeyboardModifiers modifiers;
    };

  virtual UsedFlags mouseEvent(const MouseEvent &) { return NotUsed; }

  virtual UsedFlags wheelEvent(const WheelEvent &) { return NotUsed; }

private:
  X_DISABLE_COPY(AbstractCanvasController);
  };

}

#endif // XABSTRACTCANVASCONTROLLER_H
