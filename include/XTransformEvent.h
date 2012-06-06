#ifndef XTRANSFORMEVENT_H
#define XTRANSFORMEVENT_H

#include "XProperty"
#include "XTransform.h"

class XDoodad;

class XTransformEvent
    {
public:
    XROProperty( XDoodad *, object );
    XROByRefProperty( XTransform, oldTransform );

public:
    XTransformEvent( XDoodad *doodad, const XTransform &old );
    };

#endif // XTRANSFORMEVENT_H
