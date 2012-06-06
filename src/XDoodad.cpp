#include "XDoodad.h"
#include "XChildEvent"
#include "XCuboid.h"
#include "XRenderer.h"
#include "XScene.h"
#include "QDebug"
#include "XTransformEvent.h"

XDoodad::XDoodad( ) : XObject( )
    {
    _scene = 0;
    }

void XDoodad::render( )
    {
    }

void XDoodad::renderFull( )
    {
    bool hasTx( testAttribute( HasTransform ) );
    if( hasTx )
        {
        scene()->renderer()->pushTransform( transform() );
        }
    render( );

    if( !testAttribute( DoesntRenderChildren ) )
        {
        foreach( XDoodad *dood, _childrenDoodads )
            {
            dood->renderFull( );
            }
        }

    if( hasTx )
        {
        scene()->renderer()->popTransform();
        }
    }

XCuboid XDoodad::bounds() const
    {
    return childrenBounds() | objectBounds();
    }

XCuboid XDoodad::childrenBounds() const
    {
    XCuboid ret;
    foreach( XDoodad *dood, _childrenDoodads )
        {
        ret |= dood->bounds();
        }
    return ret;
    }

void XDoodad::setTransform( const XTransform &trans )
    {
    XTransformEvent ev( this, _transform );
    _transform = trans;
    transformEvent( &ev );

    update();
    }

XDoodad *XDoodad::parentDoodad()
    {
    return static_cast<XDoodad*>(parent());
    }

const XDoodad *XDoodad::parentDoodad() const
    {
    return static_cast<XDoodad*>(parent());
    }

XScene *XDoodad::scene()
    {
    return _scene;
    }

const XScene *XDoodad::scene() const
    {
    return _scene;
    }

XRenderer *XDoodad::renderer()
    {
    return scene()->renderer();
    }

XCamera *XDoodad::camera()
    {
    return scene()->camera();
    }

void XDoodad::transformEvent( XTransformEvent *event )
    {
    if( parentDoodad() )
        {
        if( testAttribute( HasTransform ) )
            {
            _globalTransform = parentDoodad()->globalTransform() * transform();
            }
        else
            {
            _globalTransform = parentDoodad()->globalTransform();
            }
        }
    else
        {
        _globalTransform = transform();
        }

    foreach( XDoodad *dood, _childrenDoodads )
        {
        dood->transformEvent( event );
        }
    }

void XDoodad::childEvent( XChildEvent *event )
    {
    if( event->type() == XChildEvent::Remove )
        {
        XDoodad *child( static_cast<XDoodad*>(event->object()) );
        _childrenDoodads.removeAll( child );
        child->setScene( 0 );
        }
    else if( event->object()->castTo<XDoodad>() )
        {
        XDoodad *child( static_cast<XDoodad*>(event->object()) );
        _childrenDoodads << child;
        child->setScene( scene() );
        child->initiate();
        update();
        }
    }

void XDoodad::setScene( XScene *scene )
    {
    _scene = scene;

    if( parentDoodad() )
        {
        _globalTransform = parentDoodad()->globalTransform() * transform();
        }

    foreach( XDoodad *dood, _childrenDoodads )
        {
        dood->setScene( scene );
        }
    }

void XDoodad::setAttribute( DoodadAttribute at, bool in )
    {
    if( at >= _attributes.size() )
        {
        _attributes.resize( at+1 );
        }
    _attributes.setBit( at, in );

    if( at == HasTransform )
        {
        setTransform( transform() );
        }

    update();
    }

bool XDoodad::testAttribute( DoodadAttribute at )
    {
    if( at < _attributes.size() )
        {
        return _attributes.testBit( at );
        }
    return false;
    }

void XDoodad::initiate()
    {
    }

void XDoodad::update()
    {
    if( scene() )
        {
        trigger( scene()->update() );
        }
    }
