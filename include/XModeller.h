#ifndef XMODELLER_H
#define XMODELLER_H

#include "X3DGlobal.h"
#include "XAllocatorBase"
#include "XMathVector"
#include "XGeometry.h"
#include "XColour"
#include "XTransform.h"
#include "XShader.h"

namespace Eks
{

template <typename T> class AbstractCurve;
class Cuboid;
class ShaderVertexLayoutDescription;

class EKS3D_EXPORT Modeller
  {
XProperties:
  XROProperty(bool, areTriangleIndicesSequential);
  XROProperty(bool, areLineIndicesSequential);

public:
  Modeller(AllocatorBase *, xsize initialSize);
  ~Modeller();

  void bakeVertices(
      Renderer *r,
      ShaderVertexLayoutDescription::Semantic *semanticOrder,
      xsize semanticCount,
      Geometry *geo);

  void bakeTriangles(
      Renderer *r,
      ShaderVertexLayoutDescription::Semantic *semanticOrder,
      xsize semanticCount,
      IndexGeometry *index,
      Geometry *geo);

  void bakeLines(
      Renderer *r,
      ShaderVertexLayoutDescription::Semantic *semanticOrder,
      xsize semanticCount,
      IndexGeometry *index,
      Geometry *geo);

  // Fixed Functionality GL Emulation
  enum Type { None, Quads, Triangles, Lines };
  void begin( Type = Triangles );
  void end( );

  void vertex( const Vector3D & );
  inline void vertex( Real, Real, Real );
  void normal( const Vector3D & );
  inline void normal( Real, Real, Real );
  void texture( const Vector2D & );
  inline void texture( Real, Real );
  void colour( const Eks::Vector4D & );
  inline void colour( Real, Real, Real, Real = 1.0 );

  void setNormalsAutomatic( bool=true );
  bool normalsAutomatic( ) const;

  // Draw Functions
  void drawWireCube(const Cuboid &cube);
  void drawWireCircle(const Vector3D &pos, const Vector3D &normal, float radius, xsize pts=24);

  void drawCone(
      const Vector3D &point,
      const Vector3D &direction,
      float length,
      float radius,
      xuint32 divs=6);

  void drawSphere(float radius, int lats = 8, int longs = 12);
  void drawCube(
      const Vector3D &horizontal=Vector3D(1,0,0),
      const Vector3D &vertical=Vector3D(0,1,0),
      const Vector3D &depth=Vector3D(0,0,1),
      float tX=0.0,
      float tY=0.0 );
  void drawQuad(
      const Vector3D &horizontal=Vector3D(1,0,0),
      const Vector3D &vertical=Vector3D(0,1,0) );
  void drawLocator(
      const Vector3D &size=Vector3D(1,1,1),
      const Vector3D &center=Vector3D() );

  void drawCurve(const AbstractCurve<Vector3D> &, xsize segments );

  void setTransform( const Transform & );
  Transform transform( ) const;

  void save();
  void restore();

private:
  inline Vector3D transformPoint(const Vector3D & );
  inline void transformPoints(Vector <Vector3D> & );

  inline Vector3D transformNormal( Vector3D );
  inline void transformNormals(Vector <Vector3D> &, bool reNormalize );

  AllocatorBase *_allocator;

  Vector<xuint16> _triIndices;
  Vector<xuint16> _linIndices;

  Vector<Vector3D> _vertex;
  Vector<Vector2D> _texture;
  Vector<Vector3D> _normals;
  Vector<Vector4D> _colours;

  struct State
    {
    State() : normal(Vector3D::Zero()),
      texture(Vector2D::Zero()),
      colour(0.0f, 0.0f, 0.0f, 0.0f),
      type( None ),
      normalsAutomatic( false )
      {
      }
    Vector3D normal;
    Vector2D texture;
    Eks::Vector4D colour;
    Type type;
    bool normalsAutomatic;
    };
  Vector<State> _states;

  Transform _transform;
  int _quadCount;
  };

void Modeller::vertex( Real x, Real y, Real z )
  { vertex( Vector3D(x,y,z) ); }

void Modeller::normal( Real x, Real y, Real z )
  { normal( Vector3D(x,y,z) ); }

void Modeller::texture( Real x, Real y )
  { texture( Vector2D(x,y) ); }

void Modeller::colour( Real x, Real y, Real z, Real w )
  { colour( Eks::Vector4D(x,y,z,w) ); }

}

#endif // XMODELLER_H
