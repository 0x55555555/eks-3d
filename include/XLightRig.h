#ifndef XLIGHTRIG_H
#define XLIGHTRIG_H

#include "X3DGlobal.h"

class XLightRig
  {
public:
  virtual ~XLightRig();
  virtual xuint16 numberOfLights() const = 0;
  };

#endif // XLIGHTRIG_H
