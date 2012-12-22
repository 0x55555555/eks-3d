#ifndef XOBJLOADER_H
#define XOBJLOADER_H

#include "X3DGlobal.h"
#include "XVector3D"
#include "XSimpleString"
#include "XShader.h"

class XGeometry;
class XRenderer;
class XIndexGeometry;

class EKS3D_EXPORT XObjLoader
  {
public:
  enum
    {
    ExpectedVertices = 1024,
    ExpectedLineLength = 1024,
    MaxComponent = 3,
    MaxElements = 2
    };

  struct ObjElement;
  typedef Eigen::Matrix<xReal, MaxComponent, 1> ElementVector;
  struct ElementData
    {
    XVector<ElementVector> data;
    const XObjLoader::ObjElement *desc;
    };

  XObjLoader(XAllocatorBase *allocator=XGlobalAllocator::instance());

  void load(const char *data,
    xsize dataSize,
    const XShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    XVector<XVectorI3D> *triangles, xsize *vertexSize,
    ElementData *elements);

  void bake(const XVector<XVectorI3D> &triangles,
    const ElementData *elementData,
    xsize elementCount,
    XVector<xuint8> *dataOut);

  const ObjElement *findObjectDescriptionForSemantic(XShaderVertexLayoutDescription::Semantic s);

private:
  bool findElementType(
    const XVector<char> &line,
    const XShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    xsize *foundItem);

  bool readIndices(const XVector<char> &, xsize start, xsize *end, XVectorI3D &indices);

  XAllocatorBase *_allocator;
  XString _scratchString;
  };

#endif // XOBJLOADER_H
