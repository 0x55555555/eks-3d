#include "XObjLoader.h"
#include "XGeometry.h"

const char space = ' ';
const char separator = '/';

namespace
{

int skipSpaces(const XVector<char> &line, xsize from, xsize &firstSpace)
  {
  firstSpace = from;

  bool seenSpace = line[0] == ' ';
  while(!seenSpace || line[from] == ' ')
    {
    if(!seenSpace)
      {
      firstSpace = from;
      }

    if(line[from] == ' ')
      {
      seenSpace = true;
      }

    from++;
    if(from >= line.size())
      {
      return X_SIZE_SENTINEL;
      }
    }

  return from;
  }

template <xsize MaxCount> void writeVector(
    const XObjLoader::ElementVector &elem,
    XVector<xuint8> *data)
  {
  const xsize oldEnd = data->size();
  const xsize expandSize = sizeof(XObjLoader::ElementVector::Scalar) * MaxCount;

  const xuint8* elemData = (const xuint8*)elem.data();

  data->resizeAndCopy(oldEnd + expandSize, elemData);
  }

template <xsize MaxCount>
void readVector(
    XString &scratch,
    const XVector<char> &arr,
    xsize start,
    XVector<XObjLoader::ElementVector>* data)
  {
  XVector3D ret = XVector3D::Zero();

  xsize count = 0;
  xsize pos = start;
  xsize end = start-1;
  xsize firstSpace = 0;
  while((end = arr.indexOf(space, pos + 1)) != X_SIZE_SENTINEL && count < MaxCount)
    {
    scratch.fromUtf8(arr, pos, end - pos);

    ret(count++) = scratch.toType<xReal>();

    pos = skipSpaces(arr, pos, firstSpace);
    }

  (*data) << ret;
  }
}

struct XObjLoader::ObjElement
  {
  const char* name;
  xsize components;
  void (*read)(XString &scratch, const XVector<char> &line, xsize index, XVector<ElementVector>* data);
  void (*write)(const ElementVector &elem, XVector<xuint8> *data);
  };

const XObjLoader::ObjElement elementDescriptionsImpl[] =
  {
    { "v", 3, readVector<3>, writeVector<3>  },
    { "n", 3, readVector<3>, writeVector<3> },
    { "vt", 2, readVector<2>, writeVector<2>  },
  };

const XObjLoader::ObjElement *elementDescriptions[] =
  {
  &elementDescriptionsImpl[0], // pos
  0,                          // colour
  &elementDescriptionsImpl[2], // tex
  &elementDescriptionsImpl[1], // normal
  };

xCompileTimeAssert(X_ARRAY_COUNT(elementDescriptions) == XShaderVertexLayoutDescription::SemanticCount);

XObjLoader::XObjLoader(XAllocatorBase *allocator)
    : _allocator(allocator),
      _scratchString(_allocator)
  {
  }

const XObjLoader::ObjElement *XObjLoader::findObjectDescriptionForSemantic(XShaderVertexLayoutDescription::Semantic s)
  {
  return elementDescriptions[s];
  }

void XObjLoader::bake(
    const XVector<XVectorI3D>& unbakedTriangles,
    const ElementData *elements,
    xsize elementCount,
    XVector<xuint8> *bakedData)
  {
  for(int i = 0, s = unbakedTriangles.size(); i < s; ++i)
    {
    const XVectorI3D idx = unbakedTriangles[i];

    for(xsize elIdx = 0; elIdx < elementCount; ++elIdx)
      {
      const ElementData &element(elements[elIdx]);
      element.desc->write(element.data[idx(0)], bakedData);
      }
    }
  }

bool XObjLoader::readIndices(const XVector<char> &arr, xsize start, xsize *end, XVectorI3D &indices)
  {
  indices = XVectorI3D::Zero();

  xsize firstSpace = start;
  xsize nextSpace = skipSpaces(arr, start, firstSpace);
  if(nextSpace == X_SIZE_SENTINEL)
    {
    nextSpace = arr.length();
    }

  xsize count = 0;
  xsize pos = start;
  xsize sepEnd = start;
  while((sepEnd = arr.indexOf(separator, sepEnd+1)) != X_SIZE_SENTINEL && count < 3 && sepEnd < nextSpace)
    {
    _scratchString.fromUtf8(arr, pos, sepEnd - pos);

    bool ok = true;
    int val = _scratchString.toType<int>(&ok);
    indices(count++) = ok ? (val - 1) : 0;

    pos = sepEnd + 1;
    }
  
  if(count == 0 && pos >= firstSpace)
    {
    return false;
    }

  _scratchString.fromUtf8(arr, pos, firstSpace - pos);
  
  bool ok = true;
  int val = _scratchString.toType<int>(&ok);
  indices(count++) = ok ? (val - 1) : 0;

  *end = nextSpace;
  return true;
  }

bool XObjLoader::findElementType(
    const XVector<char> &line,
    const XShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    xsize *foundItem)
  {
  *foundItem = X_SIZE_SENTINEL;
  for(xsize i = 0; i < itemCount; ++i)
    {
    const XShaderVertexLayoutDescription::Semantic item = items[i];
    const ObjElement* element = elementDescriptions[item];
    xsize len = strlen(element->name);

    if(line.compare(element->name, len) && line[len] == space)
      {
      *foundItem = i;
      return true;
      }
    }
  return false;
  }

void XObjLoader::load(
    const char *data,
    xsize dataSize,
    const XShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    XVector<XVectorI3D> *tris,
    xsize *vertexSize,
    ElementData *elementData)
  {
  xAssert(tris);
  xAssert(vertexSize);
  xAssert(elementData);
  xAssert(itemCount <= MaxElements);

  XVector<char> line(ExpectedLineLength, ' ', _allocator);

  *vertexSize = 0;
  for(xsize i = 0; i < itemCount; ++i)
    {
    XShaderVertexLayoutDescription::Semantic semantic = items[i];
    const ObjElement *el = elementDescriptions[semantic];

    if(el != 0)
      {
      elementData[i].desc = el;
      elementData[i].data.setAllocator(_allocator);
      elementData[i].data.reserve(ExpectedVertices);
      }
    }


  for(xsize i = 0; i < itemCount; ++i)
    {
    const ObjElement *el = elementData[i].desc;
    if(!el)
      {
      xAssertFail();
      return;
      }

    *vertexSize += sizeof(float) * el->components;
    }

  XVectorI3D indices;

  XVector<XVectorI3D> tempPoly(_allocator);

  const char *pos = data;
  while(pos < (data + dataSize))
    {
    line.clear();

    while(*pos != '\n' && pos < (data + dataSize))
      {
      line << *pos;
      ++pos;
      }
    ++pos; // skip \n
    
    // remove the carriage return from win files.
    if(line.back() == '\r')
      {
      line.popBack();
      }

    // add a space to then end
    line << space;

    xsize firstSpace = 1;
    xsize nonSpace = 1;
    if(line.length() > 1)
      {
      nonSpace = skipSpaces(line, 1, firstSpace);
      }

    xsize foundItem = 0;
    if(findElementType(line, items, itemCount, &foundItem))
      {
      xAssert(foundItem < XShaderVertexLayoutDescription::SemanticCount);

      ElementData &data = elementData[foundItem];
      const ObjElement *element(data.desc);
      xAssert(element);

      element->read(_scratchString, line, nonSpace, &(data.data));
      }
    else if(line.compare("f ", 2))
      {
      tempPoly.clear();
      // face
      xsize pos = nonSpace;
      bool valid = false;
      while(
          pos < line.length() &&
          (valid = readIndices(line, pos, &pos, indices)) == true &&
          valid)
        {
        tempPoly << indices;
        }

      for(xsize i=0; i < tempPoly.size() - 2; ++i)
        {
        (*tris) << tempPoly[0] << tempPoly[i+1] << tempPoly[i+2];
        }
      }
    }
  }

/*
  if(computeUnusedEntries)
    {
    for(xsize i = 0; i < itemCount; ++i)
      {
      ElementData &element = elementData[i];
      if(element.data.size() == 0)
        {
        if(strcmp(element.desc->name, "n") == 0)
          {
          xAssertFail();
          }
        }
      }
//    nor.resize(vtx.size());
//    for(xsize i = 0; i < ((xsize)bakedTris.size()-2); i+=3)
//      {
//      XVector3D pts[3];
//      for(xsize j = 0; j < 3; ++j)
//        {
//        xsize idx = i + j;
//        xsize vtxIdx = bakedTris[idx];
//        pts[j] = vtx[vtxIdx];
//        }

//      XVector3D normal = (pts[1] - pts[0]).cross(pts[2] - pts[0]).normalized();

//      nor[i] = nor[i+1] = nor[i+2] = normal;
//      }
    }*/
