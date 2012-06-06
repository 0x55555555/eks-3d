#ifndef XFRAMEEVENT_H
#define XFRAMEEVENT_H

#include "X3DGlobal.h"
#include "XProperty"

class EKS3D_EXPORT XFrameEvent
    {
public:
    enum Mode { RenderBegin, RenderFinished };
    XROProperty( Mode, mode );

public:
    XFrameEvent( Mode );
    };

#endif // XFRAMEEVENT_H
