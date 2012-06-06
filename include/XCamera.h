#ifndef XCAMERA_H
#define XCAMERA_H

#include "X3DGlobal.h"
#include "XFrustum.h"
#include "XObject"
#include "XProperty"
#include "XVector3D"
#include "XMatrix4x4"
#include "QSize"
#include "QPoint"

class XScene;

class EKS3D_EXPORT XCamera : public XObject
    {
    X_OBJECT( XCamera, XObject, 5 )
public:
    XRORefProperty( XTransform, viewTransform );
    XRORefProperty( XComplexTransform, projectionTransform );

    XRORefProperty( XVector3D, position );
    XRORefProperty( XVector3D, aimPosition );
    XRORefProperty( XVector3D, upDirection );

    XROProperty( QSize, viewportSize );
    XROProperty( xReal, aspectRatio );

    XProperty( XScene *, currentScene, setCurrentScene );

public:
    XCamera( XVector3D position, XVector3D aimPosition=XVector3D( 0, 0, 0 ), XVector3D up=XVector3D( 0, 1, 0 ) );

    void setUpDirection( XVector3D );
    void setPosition( XVector3D );
    void setAimDirection( XVector3D );
    void setAimPosition( XVector3D );

    void setViewportSize( const QSize & );

    XVector3D screenToWorld( const QPoint & ) const;

    XVector3D aimDirection() const;
    XVector3D vertical() const;
    XVector3D horizontal() const;

    void pan( xReal x, xReal y );
    void track( xReal x, xReal y, xReal z );

    X_SIGNAL( viewChanged );
    X_SIGNAL( projectionChanged );

    X_ALIGNED_OPERATOR_NEW

protected:
    void update();

    void setProjectionTransform( const XComplexTransform & );
    virtual void aspectRatioChanged();
    virtual void viewTransformChanged();

private:
    void setTransform();
    bool _invertedTransformIsValid;
    mutable XComplexTransform _inverted;
    };

class EKS3D_EXPORT XPerspectiveCamera : public XCamera
    {
    X_OBJECT( XCamera, XObject, 6 )
public:
    XROProperty( xReal, viewAngle )
    XROProperty( xReal, nearClipPlane )
    XROProperty( xReal, farClipPlane )

    XRORefProperty( XFrustum, frustum );

public:
    XPerspectiveCamera( xReal angle, XVector3D position, XVector3D aimPosition=XVector3D( 0, 0, 0 ), XVector3D up=XVector3D( 0, 1, 0 ), xReal near=1.0, xReal far=1000.0 );

    void setViewAngle( xReal );
    void setNearClipPlane( xReal );
    void setFarClipPlane( xReal );

private:
    void viewTransformChanged();
    void aspectRatioChanged();
    void updateProjectionTransform();
    };

#endif // XCAMERA_H
