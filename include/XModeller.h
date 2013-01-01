#ifndef XMODELLER_H
#define XMODELLER_H

#include "X3DGlobal.h"
#include "XMathVector"
#include "XGeometry.h"
#include "XColour"
#include "XTransform.h"

#if 0

template <typename T> class XAbstractCurve;

class EKS3D_EXPORT XModeller
    {
public:
    XModeller( Geometry *, xsize initialSize );
    ~XModeller( );

    void flush( );

    // Fixed Functionality GL Emulation
    enum Type { None, Quads, Triangles, Lines, Points };
    void begin( Type = Triangles );
    void end( );

    void vertex( const Eks::Vector3D & );
    inline void vertex( Real, Real, Real );
    void normal( const Eks::Vector3D & );
    inline void normal( Real, Real, Real );
    void texture( const Eks::Vector2D & );
    inline void texture( Real, Real );
    void colour( const Eks::Vector4D & );
    inline void colour( Real, Real, Real, Real = 1.0 );

    void setNormalsAutomatic( bool=true );
    bool normalsAutomatic( ) const;

    // Draw Functions
    void drawGeometry( const Geometry &, bool normaliseNormals=false );
    void drawGeometry( XList <Eks::Vector3D> positions, const Geometry &, bool normaliseNormals=false );

    void drawWireCube( const XCuboid &cube );

    void drawCone(const Eks::Vector3D &point, const Eks::Vector3D &direction, float length, float radius, xuint32 divs=6);

    void drawSphere(float radius, int lats = 8, int longs = 12);
    void drawCube( Eks::Vector3D horizontal=Eks::Vector3D(1,0,0), Eks::Vector3D vertical=Eks::Vector3D(0,1,0), Eks::Vector3D depth=Eks::Vector3D(0,0,1), float tX=0.0, float tY=0.0 );
    void drawQuad( Eks::Vector3D horizontal=Eks::Vector3D(1,0,0), Eks::Vector3D vertical=Eks::Vector3D(0,1,0) );
    void drawLocator( XSize size=XSize(1,1,1), Eks::Vector3D center=Eks::Vector3D() );

    void drawCurve( const XAbstractCurve <Eks::Vector3D> &, xsize segments );

    void setTransform( const XTransform & );
    XTransform transform( ) const;

    void save();
    void restore();

private:
    inline Eks::Vector3D transformPoint( const Eks::Vector3D & );
    inline XVector <Eks::Vector3D> transformPoints( const XVector <Eks::Vector3D> & );

    inline Eks::Vector3D transformNormal( Eks::Vector3D );
    inline XVector <Eks::Vector3D> transformNormals( const XVector <Eks::Vector3D> &, bool reNormalize );

    Geometry *_geo;
    XVector <xuint32> _triIndices;
    XVector <xuint32> _linIndices;
    XVector <xuint32> _poiIndices;
    XVector <Eks::Vector3D> _vertex;
    XVector <Eks::Vector2D> _texture;
    XVector <Eks::Vector3D> _normals;
    XVector <Eks::Vector4D> _colours;

    struct State
        {
        State() : normal(Eks::Vector3D::Zero()),
            texture(Eks::Vector2D::Zero()),
            colour(0.0f, 0.0f, 0.0f, 0.0f),
            type( None ),
            normalsAutomatic( false )
          {
          }
        Eks::Vector3D normal;
        Eks::Vector2D texture;
        Eks::Vector4D colour;
        Type type;
        bool normalsAutomatic;
        };
    XVector <State> _states;

    XTransform _transform;
    int _quadCount;
    };

void XModeller::vertex( Real x, Real y, Real z )
    { vertex( Eks::Vector3D(x,y,z) ); }

void XModeller::normal( Real x, Real y, Real z )
    { normal( Eks::Vector3D(x,y,z) ); }

void XModeller::texture( Real x, Real y )
    { texture( Eks::Vector2D(x,y) ); }

void XModeller::colour( Real x, Real y, Real z, Real w )
    { colour( Eks::Vector4D(x,y,z,w) ); }

#endif

#endif // XMODELLER_H
