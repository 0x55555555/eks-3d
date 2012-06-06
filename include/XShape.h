#ifndef XSHAPE_H
#define XSHAPE_H

#include "X3DGlobal.h"
#include "XShared"
#include "XGeometry.h"
#include "XRenderer.h"

class XShader;
class XShapeData;

class EKS3D_EXPORT XShape
  {
public:
  XShape( XGeometry geometry=XGeometry(), int renderFlags=XRenderer::DepthTest|XRenderer::BackfaceCulling );
  XShape( XGeometry geometry, const XShader &shader, int renderFlags=XRenderer::DepthTest|XRenderer::BackfaceCulling );

  void enableRenderFlag( XRenderer::RenderFlags );
  void disableRenderFlag( XRenderer::RenderFlags );
  void setRenderFlags( int );
  int renderFlags() const;

  XGeometry &geometry();
  const XGeometry &geometry() const;

  void setShader( const XShader & );
  void clearShader( );
  bool hasShader( ) const;
  XShader *shader();
  const XShader *constShader() const;

  friend EKS3D_EXPORT QDataStream &operator<<(QDataStream &stream, const XShape &child);
  friend EKS3D_EXPORT QDataStream &operator>>(QDataStream &stream, XShape &child);

private:
  XSharedPointer<XShapeData> _d;
  };

typedef XList<XShape> XShapeList;

#endif // XSHAPE_H
