#ifndef XMODELLER_H
#define XMODELLER_H

#include "X3DGlobal.h"
#include "XVector2D"
#include "XVector3D"
#include "XList"
#include "XGeometry.h"
#include "XSize"
#include "XColour"
#include "XTransform.h"

template <typename T> class XAbstractCurve;

class EKS3D_EXPORT XModeller
    {
public:
    XModeller( XGeometry *, xsize initialSize );
    ~XModeller( );

    void flush( );

    // Fixed Functionality GL Emulation
    enum Type { None, Quads, Triangles, Lines, Points };
    void begin( Type = Triangles );
    void end( );

    void vertex( const XVector3D & );
    inline void vertex( xReal, xReal, xReal );
    void normal( const XVector3D & );
    inline void normal( xReal, xReal, xReal );
    void texture( const XVector2D & );
    inline void texture( xReal, xReal );
    void colour( const XVector4D & );
    inline void colour( xReal, xReal, xReal, xReal = 1.0 );

    void setNormalsAutomatic( bool=true );
    bool normalsAutomatic( ) const;

    // Draw Functions
    void drawGeometry( const XGeometry & );
    void drawGeometry( XList <XVector3D> positions, const XGeometry & );

    void drawWireCube( const XCuboid &cube );

    void drawCone(const XVector3D &point, const XVector3D &direction, float length, float radius, xuint32 divs=6);

    void drawCube( XVector3D horizontal=XVector3D(1,0,0), XVector3D vertical=XVector3D(0,1,0), XVector3D depth=XVector3D(0,0,1), float tX=0.0, float tY=0.0 );
    void drawQuad( XVector3D horizontal, XVector3D vertical );
    void drawLocator( XSize size=XSize(1,1,1), XVector3D center=XVector3D() );

    void drawCurve( const XAbstractCurve <XVector3D> &, xsize segments );

    void setTransform( const XTransform & );
    XTransform transform( ) const;

    void save();
    void restore();

private:
    inline XVector3D transformPoint( const XVector3D & );
    inline XVector <XVector3D> transformPoints( const XVector <XVector3D> & );

    inline XVector3D transformNormal( XVector3D );
    inline XVector <XVector3D> transformNormals( const XVector <XVector3D> & );

    XGeometry *_geo;
    XVector <xuint32> _triIndices;
    XVector <xuint32> _linIndices;
    XVector <xuint32> _poiIndices;
    XVector <XVector3D> _vertex;
    XVector <XVector2D> _texture;
    XVector <XVector3D> _normals;
    XVector <XVector4D> _colours;

    struct State
        {
        State() : normal(XVector3D::Zero()),
            texture(XVector2D::Zero()),
            colour(1.0f, 1.0f, 1.0f, 1.0f),
            type( None ),
            normalsAutomatic( false )
          {
          }
        XVector3D normal;
        XVector2D texture;
        XVector4D colour;
        Type type;
        bool normalsAutomatic;
        };
    XVector <State> _states;

    XTransform _transform;
    int _quadCount;
    };

void XModeller::vertex( xReal x, xReal y, xReal z )
    { vertex( XVector3D(x,y,z) ); }

void XModeller::normal( xReal x, xReal y, xReal z )
    { normal( XVector3D(x,y,z) ); }

void XModeller::texture( xReal x, xReal y )
    { texture( XVector2D(x,y) ); }

void XModeller::colour( xReal x, xReal y, xReal z, xReal w )
    { colour( XVector4D(x,y,z,w) ); }

#endif // XMODELLER_H
