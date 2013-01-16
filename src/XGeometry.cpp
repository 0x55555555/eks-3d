#include "XGeometry.h"
#include "XRenderer.h"
#include "XTriangle.h"
#include "XCuboid.h"

namespace Eks
{

Geometry::Geometry(Renderer *r, const void *data, xsize elementSize, xsize elementCount)
    : _renderer(0)
  {
  if(data)
    {
    xAssert(elementSize && elementCount);
    delayedCreate(*this, r, data, elementSize, elementCount);
    }
  }

Geometry::~Geometry()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.geometry(_renderer, this);
    }
  }

bool Geometry::delayedCreate(
    Geometry &ths,
    Renderer *r,
    const void *data,
    xsize elementSize,
    xsize elementCount)
  {
  ths._renderer = r;
  return r->functions().create.geometry(r, &ths, data, elementSize, elementCount);
  }

IndexGeometry::IndexGeometry(Renderer *r, Type type, const void *data, xsize dataSize)
    : _renderer(0)
  {
  if(data)
    {
    xAssert(dataSize);
    delayedCreate(*this, r, type, data, dataSize);
    }
  }

IndexGeometry::~IndexGeometry()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.indexGeometry(_renderer, this);
    }
  }

bool IndexGeometry::delayedCreate(
    IndexGeometry &ths,
    Renderer *r,
    Type type,
    const void *index,
    xsize indexCount)
  {
  ths._renderer = r;
  return r->functions().create.indexGeometry(r, &ths, type, index, indexCount);
  }

}

/*

namespace XMeshUtilities
{
bool intersect( QString semantic,
                const XLine &ray,
                const XGeometry &geo,
                XVector3DList *posOut,
                XList <unsigned int> *triOut )
  {
  if(posOut)
    {
    posOut->clear();
    }
  if(triOut)
    {
    triOut->clear();
    }

  xAssert( geo.attributes3D().contains(semantic) );
  const XVector<XVector3D> &positions = geo.attributes3D()[semantic];
  const XVector<unsigned int> &tris = geo.triangles();

  xAssert( tris.size()%3 == 0 );
  unsigned int trianglesSize = tris.size()/3;
  for( unsigned int index=0; index<trianglesSize; ++index )
    {
    const XVector3D &a(positions[tris[index*3]]),
                              &b(positions[tris[(index*3)+1]]),
                              &c(positions[tris[(index*3)+2]]);

    XVector3D pos;
    if( XTriangle(a, b, c).intersects(ray, pos) )
      {
      if(posOut)
        {
        *posOut << pos;
        if(triOut)
          {
          *triOut << index;
          }
        }
      else
        {
        return true;
        }
      }
    }

  return posOut ? posOut->size() > 0 : false;
  }
}

*/
