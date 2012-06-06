#include "XScene.h"
#include "XFrameEvent.h"
#include "XRenderer.h"
#include "XCamera.h"

#include "QDebug"

XScene::XScene( XRenderer *renderer, XCamera *cam ) : _renderer( renderer ), _camera( cam ), _root( new XDoodad( ) )
    {
    _root->setScene( this );

    if( _camera )
        {
        _camera->setCurrentScene( this );
        }
    }

XScene::~XScene()
    {
    delete _root;
    }

void XScene::addDoodad( XDoodad *doodad )
    {
    _root->addChild( doodad );

    trigger( update() );
    }

void XScene::renderScene( )
    {
    if( _root && renderer() && camera() )
        {
        renderer()->setViewportSize( _viewportSize );
        renderer()->clear();

        renderer()->setProjectionTransform( camera()->projectionTransform() );

        renderer()->pushTransform( camera()->viewTransform() );

        XFrameEvent ev1( XFrameEvent::RenderBegin );
        trigger( frameEvent( &ev1 ) );

        _root->renderFull( );

        XFrameEvent ev2( XFrameEvent::RenderFinished );
        trigger( frameEvent( &ev2 ) );

        renderer()->popTransform();
        }
    else
        {
        qDebug() << "Couldnt render";
        }
    }

void XScene::setViewportSize( QSize size )
    {
    _viewportSize = size;

    renderer()->setViewportSize( size );

    if( _camera )
        {
        _camera->setViewportSize( size );
        }
    trigger( update() );
    }

void XScene::setCamera( XCamera *cam )
    {
    if( _camera )
        {
        _camera->setCurrentScene( 0 );
        }

    _camera = cam;

    if( _camera )
        {
        _camera->setCurrentScene( this );
        _camera->setViewportSize( _viewportSize );
        }
    trigger( update() );
    }
