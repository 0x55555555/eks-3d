#ifndef XDOODAD_H
#define XDOODAD_H

#include "X3DGlobal.h"
#include "XObject"
#include "XTransform.h"
#include "XProperty"
#include "XCuboid.h"
#include "QBitArray"

class XScene;
class XFrameEvent;
class XTransformEvent;
class XDoodad;
class XRenderer;
class XCamera;
class XRenderer;

typedef XList<XDoodad *> XDoodadList;

class EKS3D_EXPORT XDoodad : public XObject
    {
    X_OBJECT( XDoodad, XObject, 2 )

public:
    XProperty( XCuboid, objectBounds, setObjectBounds );
    XROProperty( XTransform, transform );
    XROProperty( XTransform, globalTransform );

public:
    XDoodad( );

    X_ALIGNED_OPERATOR_NEW

    virtual void render( );

    XCuboid bounds() const;
    XCuboid childrenBounds() const;

    void setTransform( const XTransform & );

    XDoodad *parentDoodad();
    const XDoodad *parentDoodad() const;
    XScene *scene();
    const XScene *scene() const;

    XRenderer *renderer();
    XCamera *camera();

    enum DoodadAttribute { HasTransform, DoesntRenderChildren };
    void setAttribute( DoodadAttribute, bool );
    bool testAttribute( DoodadAttribute );

    virtual void initiate();

protected:
    void update();

    virtual void transformEvent( XTransformEvent * );
    virtual void childEvent( XChildEvent * );

    void setScene( XScene * );

private:
    void parseChildrenDoodads();
    void renderFull( );
    XDoodadList _childrenDoodads;
    XObjectList _possibleChildrenDoodads;
    XScene *_scene;
    QBitArray _attributes;
    friend class XScene;
    };

#endif // XDOODAD_H
