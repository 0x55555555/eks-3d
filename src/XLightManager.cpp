#include "XLightManager.h"
#include "XLightRig.h"

XLightManager *XLightManager::instance()
  {
  static XLightManager ref;
  return &ref;
  }

void XLightManager::setRig( XLightRig *r )
  {
  xAssert(_rig);
  _rig = r;
  refreshAll();
  }

XLightManager::XLightManager()
  {
  }

void XLightManager::refreshAll()
  {
  xuint8 num = _rig->numberOfLights();
  trigger( numberOfLightsChanged(num) );

  for(xuint8 i=0; i<num; ++i)
    {
    trigger( lightChanged(i) );
    }
  }
