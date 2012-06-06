#ifndef XLIGHTMANAGER_H
#define XLIGHTMANAGER_H

#include "XObject"
#include "XMacroHelpers"

class XLightRig;

class XLightManager : public XObject
  {
public:
  static XLightManager *instance( );

  void setRig( XLightRig * );

  // emitted when a light changed and should be reset in shaders
  X_SIGNAL( lightChanged, xuint16 );

  // emitted when the number of lights changes
  X_SIGNAL( numberOfLightsChanged, xuint16 );

private:
  void refreshAll();

  XLightManager();
  X_DISABLE_COPY(XLightManager);

  XLightRig *_rig;
  };

#endif // XLIGHTMANAGER_H
