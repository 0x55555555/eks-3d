#ifndef ObjLoader_H
#define ObjLoader_H

#include "X3DGlobal.h"
#include "XMathVector"
#include "XStringSimple"
#include "XShader.h"

namespace Eks
{

class Geometry;
class Renderer;
class IndexGeometry;

class EKS3D_EXPORT ObjLoader
  {
public:
  enum
    {
    ExpectedVertices = 1024,
    ExpectedLineLength = 512,
    ExpectedFloatLength = 32,
    MaxComponent = 3
    };

  typedef Vector<XChar, ExpectedLineLength> LineCache;

  struct ObjElement;
  typedef Eigen::Matrix<Real, MaxComponent, 1> ElementVector;
  struct ElementData
    {
    Vector<ElementVector> data;
    const ObjLoader::ObjElement *desc;
    };

  ObjLoader(AllocatorBase *allocator=GlobalAllocator::instance());

  void load(const char *data,
    xsize dataSize,
    const ShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    Vector<VectorI3D> *triangles,
    xsize *vertexSize,
    ElementData *elements);

  void computeUnusedElements(ElementData *elements,
      xsize itemCount,
      Vector<VectorI3D> *triangles);

  void bake(const Vector<VectorI3D> &triangles,
    const ElementData *elementData,
    xsize elementCount,
    Vector<xuint8> *dataOut);

  const ObjElement *findObjectDescriptionForSemantic(ShaderVertexLayoutDescription::Semantic s);

private:
  bool findElementType(
    const LineCache &line,
    const ShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    xsize *foundItem);

  bool readIndices(
    const LineCache &,
    xsize start,
    xsize *end,
    VectorI3D &indices,
    const ElementData *elementData,
    xsize elementCount);

  Eks::AllocatorBase *_allocator;
  Eks::String _scratchString;
  };

}

#endif // ObjLoader_H
