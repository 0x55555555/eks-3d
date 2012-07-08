#ifndef XOBJLOADER_H
#define XOBJLOADER_H

#include "X3DGlobal.h"
#include "QByteArray"
#include "XVector3D"

class QIODevice;
class XGeometry;

class EKS3D_EXPORT XObjLoader
  {
public:
  XObjLoader();

  void load(QIODevice *, XGeometry*, bool fixUnusedNormals);

private:
  XVector3D readVector3D(const QByteArray &arr, int start);
  int readIndices(const QByteArray &, int start, XVectorI3D &indices);

  int skipSpaces(const QByteArray &line, int from, int &firstSpace);

  void bakeTriangles(const QVector<XVectorI3D>& unbakedTriangles,
                     QVector<xuint32> &baked,
                     QVector<XVector3D> &vtx,
                     QVector<XVector3D> &nor,
                     QVector<XVector2D> &tex);

  QByteArray _scratchString;
  };

#endif // XOBJLOADER_H
