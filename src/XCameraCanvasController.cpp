#include "XCameraCanvasController.h"
#include "QDebug"

XCameraCanvasController::XCameraCanvasController(XAbstractCanvas *canvas)
    : XAbstractCanvasController(canvas), _current(CameraInterface::None)
  {
  }


XCameraCanvasController::UsedFlags XCameraCanvasController::mouseEvent(const MouseEvent &e)
  {
  CameraInterface *cam = camera();
  if(!cam)
    {
    return NotUsed;
    }

  CameraInterface::MovementFlags supported = cam->supportedMovements();

  if(e.type == Press)
    {
    if(supported.hasFlag(CameraInterface::Track) &&
       ((e.triggerButton == Qt::MiddleButton && e.modifiers == Qt::NoModifier) || (e.triggerButton == Qt::LeftButton && e.modifiers == Qt::AltModifier)))
      {
      _current = CameraInterface::Track;
      return Used;
      }
    else if(supported.hasFlag(CameraInterface::Dolly) &&
       ((e.triggerButton == Qt::MiddleButton || e.buttonsDown == (Qt::LeftButton|Qt::RightButton)) && e.modifiers == (Qt::ShiftModifier|Qt::AltModifier)))
      {

      _current = CameraInterface::Pan;
      return Used;
      }
    else if(supported.hasFlag(CameraInterface::Dolly) &&
       (((e.triggerButton == Qt::MiddleButton || e.buttonsDown == (Qt::LeftButton|Qt::RightButton)) && e.modifiers == Qt::AltModifier) ||
            ((e.triggerButton == Qt::LeftButton) && e.modifiers == (Qt::AltModifier|Qt::ControlModifier))))
      {

      _current = CameraInterface::Dolly;
      return Used;
      }
    else if(supported.hasFlag(CameraInterface::Zoom) &&
       (e.triggerButton == Qt::RightButton && e.modifiers == Qt::AltModifier))
      {
      _zoomCentre = e.point;
      _current = CameraInterface::Zoom;
      return Used;
      }
    }
  else if(e.type == Move)
    {
    QPoint delta = e.point - lastKnownMousePosition();
    if(_current == CameraInterface::Track)
      {
      cam->track(delta.x(), delta.y());
      return Used|NeedsUpdate;
      }
    else if(_current == CameraInterface::Dolly)
      {
      cam->dolly(delta.x(), delta.y());
      return Used|NeedsUpdate;
      }
    else if(_current == CameraInterface::Pan)
      {
      cam->pan(delta.x(), delta.y());
      return Used|NeedsUpdate;
      }
    else if(_current == CameraInterface::Zoom)
      {
      QPoint delta = e.point - lastKnownMousePosition();
      float length = sqrt((float)(delta.x()*delta.x() + delta.y()*delta.y()));
      if(delta.y() < 0.0f)
        {
        length *= -1.0f;
        }
      length *= 0.01f;
      length += 1.0f;

      cam->zoom(length, _zoomCentre.x(), _zoomCentre.y());
      return Used|NeedsUpdate;
      }
    }
  else if(e.type == Release)
    {
    if(_current != CameraInterface::None)
      {
      _current = CameraInterface::None;

      return Used;
      }
    }

  return NotUsed;
  }


XCameraCanvasController::UsedFlags XCameraCanvasController::wheelEvent(const WheelEvent &w)
  {
  CameraInterface *cam = camera();
  if(!cam)
    {
    return NotUsed;
    }

  CameraInterface::MovementFlags supported = cam->supportedMovements();

  if(supported.hasFlag(CameraInterface::Zoom))
    {
    float length = 1.0f + (w.delta * 0.002f);

    cam->zoom(length, w.point.x(), w.point.y());
    return Used|NeedsUpdate;
    }

  return NotUsed;
  }
