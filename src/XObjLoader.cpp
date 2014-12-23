#include "XObjLoader.h"
#include "Containers/XStringBuilder.h"
#include "Utilities/XParseException.h"

namespace Eks
{

const char space = ' ';
const char separator = '/';

namespace
{

xsize skipSpaces(const ObjLoader::LineCache &line, xsize from, xsize &firstSpace)
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
      return std::numeric_limits<xsize>::max();
      }
    }

  return from;
  }

template <xsize MaxCount> void writeVector(
    const ObjLoader::ElementVector &elem,
    Vector<xuint8> *data)
  {
  const xsize oldEnd = data->size();
  const xsize expandSize = sizeof(ObjLoader::ElementVector::Scalar) * MaxCount;

  const xuint8* elemData = (const xuint8*)elem.data();

  data->resizeAndCopy(oldEnd + expandSize, elemData);
  }

template <xsize MaxCount>
bool readVector(
    Eks::String &scratch,
    const ObjLoader::LineCache &arr,
    xsize lineIdx,
    xsize start,
    Vector<ObjLoader::ElementVector>* data)
  {
  Eks::Vector3D ret = Eks::Vector3D::Zero();

  xsize count = 0;
  xsize pos = start;
  xsize end = start-1;
  xsize firstSpace = 0;
  while((end = arr.indexOf(space, pos + 1)) != std::numeric_limits<xsize>::max() &&
        count < MaxCount &&
        pos != std::numeric_limits<xsize>::max())
    {
    scratch.mid(arr, pos, end - pos);

    bool error = false;
    ret(count++) = scratch.toType<Real>(&error);
    if (error)
      {
      throw Eks::ParseException(X_PARSE_ERROR(
        Eks::ParseError::LineContext,
        Eks::String(arr.data(), arr.length()),
        lineIdx,
        pos,
        Eks::StringBuilder() << "Error reading number '" << scratch << "'"));
      }

    pos = skipSpaces(arr, pos, firstSpace);
    }

  (*data) << ret;
  return count == MaxCount;
  }
}

bool readAndFlipYVector2(
    Eks::String &scratch,
    const ObjLoader::LineCache &arr,
    xsize lineIdx,
    xsize start,
    Vector<ObjLoader::ElementVector>* data)
  {
  if(!readVector<2>(scratch, arr, lineIdx, start, data))
    {
    return false;
    }
  ObjLoader::ElementVector& toFlip = data->back();
  toFlip.y() = 1.0f + (-1.0f * toFlip.y());

  return true;
  }

struct ObjLoader::ObjElement
  {
  ShaderVertexLayoutDescription::Semantic semantic;
  const char* name;
  xsize components;
  bool (*read)(
      Eks::String &scratch,
      const ObjLoader::LineCache &line,
      xsize lineIdx,
      xsize index,
      Vector<ElementVector>* data);
  void (*write)(
      const ElementVector &elem,
      Vector<xuint8> *data);
  void (*compute)(
      ObjLoader::ElementData *elements,
      xsize itemCount,
      Vector<VectorI3D> *triangles,
      xsize i);
  };

namespace
{

void computeNull(
    ObjLoader::ElementData *elements,
    xsize,
    Vector<VectorI3D> *triangles,
    xsize i)
  {
  xAssertFail();
  ObjLoader::ElementData &el = elements[i];

  el.data.clear();
  el.data << ObjLoader::ElementVector::Zero();

  for(xsize triIndex = 0, s = triangles->size(); triIndex < s; ++triIndex)
    {
    VectorI3D &tri = triangles->at(triIndex);
    tri[i] = 0;
    }
  }

void computeNormal(
    ObjLoader::ElementData *elements,
    xsize itemCount,
    Vector<VectorI3D> *triangles,
    xsize elIdx)
  {
  xsize posIdx = Eks::maxFor(posIdx);
  ObjLoader::ElementData *position = 0;
  for(xsize i = 0; i < itemCount; ++i)
    {
    ObjLoader::ElementData *data = &elements[i];
    if(data->desc->semantic == ShaderVertexLayoutDescription::Position)
      {
      posIdx = i;
      position = data;
      }
    }

  if(!position)
    {
    xAssertFail();
    return;
    }

  ObjLoader::ElementData &el = elements[elIdx];

  el.data.clear();

  xAssert((triangles->size() % 3) == 0);
  for(xsize triIndex = 0, s = triangles->size(); triIndex < s; triIndex+=3)
    {
    VectorI3D &triA = triangles->at(triIndex);
    VectorI3D &triB = triangles->at(triIndex+1);
    VectorI3D &triC = triangles->at(triIndex+2);

    xsize newNorm = el.data.size();

    Vector3D a = position->data[triA[posIdx]];
    Vector3D b = position->data[triB[posIdx]];
    Vector3D c = position->data[triC[posIdx]];

    el.data << (b-a).cross(c-a).normalized();

    triA[elIdx] = (int)newNorm;
    triB[elIdx] = (int)newNorm;
    triC[elIdx] = (int)newNorm;
    }
  }
}

const ObjLoader::ObjElement elementDescriptionsImpl[] =
  {
    { ShaderVertexLayoutDescription::Position, "v", 3, readVector<3>, writeVector<3>, computeNull  },
    { ShaderVertexLayoutDescription::Normal, "vn", 3, readVector<3>, writeVector<3>, computeNormal },
    { ShaderVertexLayoutDescription::TextureCoordinate, "vt", 2, readAndFlipYVector2, writeVector<2>, computeNull }
  };

const ObjLoader::ObjElement *elementDescriptions[] =
  {
  &elementDescriptionsImpl[0], // pos
  0,                           // colour
  &elementDescriptionsImpl[2], // tex
  &elementDescriptionsImpl[1], // normal
  0,                           // binormal
  };

xCompileTimeAssert(X_ARRAY_COUNT(elementDescriptions) == ShaderVertexLayoutDescription::SemanticCount);

ObjLoader::ObjLoader(AllocatorBase *allocator)
    : _allocator(allocator),
      _scratchString(_allocator)
  {
  _scratchString.reserve(ExpectedFloatLength);
  }

const ObjLoader::ObjElement *ObjLoader::findObjectDescriptionForSemantic(ShaderVertexLayoutDescription::Semantic s)
  {
  return elementDescriptions[s];
  }

bool ObjLoader::bake(
    const Vector<VectorI3D>& unbakedTriangles,
    const ElementData *elements,
    xsize elementCount,
    Vector<xuint8> *bakedData)
  {
  for(xsize i = 0, s = unbakedTriangles.size(); i < s; ++i)
    {
    const VectorI3D &idx = unbakedTriangles[i];

    for(xsize elIdx = 0; elIdx < elementCount; ++elIdx)
      {
      const ElementData &element(elements[elIdx]);
      xsize index = idx(elIdx);
      if (index >= element.data.size())
        {
        throw Eks::ParseException(X_PARSE_ERROR(Eks::StringBuilder() << "Error baking attribute '" << element.desc->name << "' invalid index [" << index << "/" << element.data.size() << "]"));
        }
      element.desc->write(element.data[index], bakedData);
      }
    }
  return true;
  }

bool ObjLoader::readIndices(
    const LineCache &arr,
    xsize lineIdx,
    xsize start,
    xsize *end,
    VectorI3D &indices,
    const ElementData *elementData,
    xsize elementCount)
  {
  const ShaderVertexLayoutDescription::Semantic SemanticMap[] =
    {
    ShaderVertexLayoutDescription::Position,
    ShaderVertexLayoutDescription::TextureCoordinate,
    ShaderVertexLayoutDescription::Normal
    };

  indices = VectorI3D::Zero();

  xsize firstSpace = start;
  xsize nextSpace = skipSpaces(arr, start, firstSpace);
  if(nextSpace == Eks::maxFor(nextSpace))
    {
    nextSpace = arr.length();
    }

  xsize count = 0;
  xsize pos = start;
  xsize sepEnd = start;
  bool doneLast = false;
  while(((sepEnd = arr.indexOf(separator, sepEnd+1)) != std::numeric_limits<xsize>::max() && count < 3 && sepEnd < nextSpace) || !doneLast)
    {
    if(sepEnd >= nextSpace)
      {
      doneLast = true;
      sepEnd = firstSpace;
      }
    _scratchString.mid(arr, pos, sepEnd - pos);

    ShaderVertexLayoutDescription::Semantic semantic = SemanticMap[count++];
    xsize index = Eks::maxFor(index);
    for(xsize i = 0; i < elementCount; ++i)
      {
      if(elementData[i].desc->semantic == semantic)
        {
        index = i;
        break;
        }
      }

    if(index >= elementCount)
      {
      pos = sepEnd + 1;
      continue;
      }

    if (_scratchString.length())
      {
      bool error = false;
      int val = _scratchString.toType<int>(&error);
      if(error)
        {
        throw Eks::ParseException(X_PARSE_ERROR(
          Eks::ParseError::LineContext,
          Eks::String(arr.data(), arr.length()),
          lineIdx,
          pos,
          Eks::StringBuilder() << "Failed reading index '" << _scratchString << "'"));
        }
      indices(index) = val - 1;
      }
    else
      {
      indices(index) = 0;
      }

    pos = sepEnd + 1;
    }

  if(count == 0 && pos >= firstSpace)
    {
    throw Eks::ParseException(X_PARSE_ERROR(
      Eks::ParseError::LineContext,
      Eks::String(arr.data(), arr.length()),
      lineIdx,
      pos,
      "Failed reading indices"));
    }

  *end = nextSpace;
  return true;
  }

bool ObjLoader::findElementType(
    const LineCache &line,
    const ShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    xsize *foundItem)
  {
  *foundItem = Eks::maxFor(*foundItem);
  for(xsize i = 0; i < itemCount; ++i)
    {
    const ShaderVertexLayoutDescription::Semantic item = items[i];
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

bool ObjLoader::load(
    const char *data,
    xsize dataSize,
    const ShaderVertexLayoutDescription::Semantic *items,
    xsize itemCount,
    Vector<VectorI3D> *tris,
    xsize *vertexSize,
    ElementData *elementData)
  {
  xAssert(tris);
  xAssert(vertexSize);
  xAssert(elementData);

  LineCache line(ExpectedLineLength, ' ', _allocator);
  xsize lineIdx = 0;

  *vertexSize = 0;
  for(xsize i = 0; i < itemCount; ++i)
    {
    ShaderVertexLayoutDescription::Semantic semantic = items[i];
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
      return false;
      }

    *vertexSize += sizeof(float) * el->components;
    }

  Vector<VectorI3D, 6> tempPoly(_allocator);

  const char *pos = data;
  while(pos < (data + dataSize))
    {
    line.clear();
    ++lineIdx;

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

    // add a space to the end
    line << space;

    xsize firstSpace = 1;
    xsize nonSpace = 1;
    if(line.length() > 1)
      {
      nonSpace = skipSpaces(line, 1, firstSpace);
      }

    if(nonSpace == std::numeric_limits<xsize>::max())
      {
      throw Eks::ParseException(X_PARSE_ERROR(
        Eks::ParseError::LineContext,
        Eks::String(line.data(), line.length()),
        lineIdx,
        0,
        "Failed reading line"));
      }

    xsize foundItem = 0;
    if(findElementType(line, items, itemCount, &foundItem))
      {
      xAssert(foundItem < ShaderVertexLayoutDescription::SemanticCount);

      ElementData &data = elementData[foundItem];
      const ObjElement *element(data.desc);
      xAssert(element);

      if (!element->read(_scratchString, line, lineIdx, nonSpace, &(data.data)))
        {
        throw Eks::ParseException(X_PARSE_ERROR(
          Eks::ParseError::LineContext,
          Eks::String(line.data(), line.length()),
          lineIdx,
          nonSpace,
          "Failed reading element"));
        }
      }
    else if(line.compare("f ", 2))
      {
      tempPoly.clear();
      // face
      xsize pos = nonSpace;
      bool valid = false;
      VectorI3D indices;
      while(
          pos < line.length() &&
          (valid = readIndices(line, lineIdx, pos, &pos, indices, elementData, itemCount)) == true &&
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

  return true;
  }

void ObjLoader::computeUnusedElements(
    ObjLoader::ElementData *elements,
    xsize itemCount,
    Vector<VectorI3D> *triangles)
  {
  for(xsize i = 0; i < itemCount; ++i)
    {
    ObjLoader::ElementData &el = elements[i];
    if(el.data.size() != 0)
      {
      continue;
      }

    el.desc->compute(elements, itemCount, triangles, i);
    }
  }

}
