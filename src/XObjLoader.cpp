#include "XObjLoader.h"
#include "QIODevice"
#include "XGeometry.h"

#if 0

XObjLoader::XObjLoader()
  {
  }

XVector3D XObjLoader::readVector3D(const QByteArray &arr, int start)
  {
  XVector3D ret = XVector3D::Zero();

  xsize count = 0;
  int pos = start;
  int end = start-1;
  int firstSpace = 0;
  while((end = arr.indexOf(" ", pos + 1)) != -1 && count < 3)
    {
    _scratchString = arr.mid(pos, end - pos);

    ret(count++) = _scratchString.toFloat();

    pos = skipSpaces(arr, pos, firstSpace);
    }

  return ret;
  }

void XObjLoader::bakeTriangles(const QVector<XVectorI3D>& unbakedTriangles,
                               QVector<xuint32> &baked,
                               QVector<XVector3D> &vtx,
                               QVector<XVector3D> &nor,
                               QVector<XVector2D> &tex)
  {
  QVector<XVector3D> vtxO;
  QVector<XVector3D> norO;
  QVector<XVector2D> texO;

  for(int i = 0, s = unbakedTriangles.size(); i < s; ++i)
    {
    const XVectorI3D idx = unbakedTriangles[i];

    baked << i;
    if(vtx.size())
      {
      vtxO << vtx[idx(0)];
      }
    
    if(tex.size())
      {
      texO << tex[idx(1)];
      }

    if(nor.size())
      {
      norO << nor[idx(2)];
      }
    }

  vtx = vtxO;
  nor = norO;
  tex = texO;
  }

int XObjLoader::readIndices(const QByteArray &arr, int start, XVectorI3D &indices)
  {
  indices = XVectorI3D::Zero();

  int firstSpace = start;
  int nextSpace = skipSpaces(arr, start, firstSpace);
  if(nextSpace == -1)
    {
    nextSpace = arr.length();
    }

  xsize count = 0;
  int pos = start;
  int end = start;
  while((end = arr.indexOf("/", end+1)) != -1 && count < 3 && end < nextSpace)
    {
    _scratchString = arr.mid(pos, end - pos);

    bool ok = true;
    int val = _scratchString.toInt(&ok);
    indices(count++) = ok ? (val - 1) : 0;

    pos = end + 1;
    }
  
  if(count == 0 && pos >= firstSpace)
    {
    return -1;
    }

  _scratchString = arr.mid(pos, firstSpace - pos);
  
  bool ok = true;
  int val = _scratchString.toInt(&ok);
  indices(count++) = ok ? (val - 1) : 0;

  return nextSpace;
  }

int XObjLoader::skipSpaces(const QByteArray &line, int from, int &firstSpace)
  {
  firstSpace = from;

  bool seenSpace = false;
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
      return -1;
      }
    }

  return from;
  }

void XObjLoader::load(QIODevice *dev, XGeometry *geo, bool fixUnusedNormals)
  {
  QByteArray line;
  line.resize(1024);

  XVector<XVector3D> vtx;
  XVector<XVector3D> nor;
  XVector<XVector2D> tex;

  XVectorI3D indices;

  XVector<XVectorI3D> tempPoly;

  XVector<XVectorI3D> tris;

  while(dev->bytesAvailable())
    {
    line.resize(1024);
    qint64 read = dev->readLine(line.data(), line.size());

    int firstSpace = 1;
    int nonSpace = skipSpaces(line, 1, firstSpace);

    // CR into space
    line[(int)read-2] = ' ';
    line.resize(read);

    if(line.startsWith("v "))
      {
      // vertex
      vtx << readVector3D(line, nonSpace);
      }
    else if(line.startsWith("vt "))
      {
      // texture
      tex << readVector3D(line, nonSpace).head<2>();
      }
    else if(line.startsWith("vn "))
      {
      // normal
      nor << readVector3D(line, nonSpace);
      }
    else if(line.startsWith("f "))
      {
      tempPoly.clear();
      // face
      int pos = nonSpace;
      while((pos = readIndices(line, pos, indices)) != -1)
        {
        tempPoly << indices;
        }

      for(int i=0; i < tempPoly.size() - 2; ++i)
        {
        tris << tempPoly[0] << tempPoly[i+1] << tempPoly[i+2];
        }
      }
    }

  XVector<xuint32> bakedTris;

  bakeTriangles(tris, bakedTris, vtx, nor, tex);

  if(vtx.size())
    {
    geo->setAttribute("vertex", vtx);
    }

  if(fixUnusedNormals && nor.size() == 0)
    {
    nor.resize(vtx.size());
    for(xsize i = 0; i < ((xsize)bakedTris.size()-2); i+=3)
      {
      XVector3D pts[3];
      for(xsize j = 0; j < 3; ++j)
        {
        xsize idx = i + j;
        xsize vtxIdx = bakedTris[idx];
        pts[j] = vtx[vtxIdx];
        }

      XVector3D normal = (pts[1] - pts[0]).cross(pts[2] - pts[0]).normalized();

      nor[i] = nor[i+1] = nor[i+2] = normal;
      }
    }

  if(nor.size())
    {
    geo->setAttribute("normalData", nor);
    }

  if(tex.size())
    {
    geo->setAttribute("textureData", tex);
    }

  geo->setTriangles(bakedTris);
  }

#endif
