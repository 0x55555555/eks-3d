#ifndef XGEOMETRY_H
#define XGEOMETRY_H

#include "X3DGlobal.h"
#include "XMetaType"
#include "XHash"
#include "XList"
#include "XSet"
#include "XVector"
#include "XVector2D"
#include "XVector3D"
#include "XVector4D"
#include "XObject"
#include "XTransform.h"

class XAbstractGeometry;
class XRenderer;
class XCuboid;
class XLine;

class EKS3D_EXPORT XGeometry
    {
public:
    enum BufferType { Static, Dynamic, Stream };
    XGeometry( BufferType=Static );
    XGeometry( const XGeometry & );
    XGeometry& operator=( const XGeometry & );

    const XVector<unsigned int> &points() const;
    const XVector<unsigned int> &lines() const;
    const XVector<unsigned int> &triangles() const;

    const XHash <QString, XVector<xReal> > &attributes1D() const;
    const XHash <QString, XVector<XVector2D> > &attributes2D() const;
    const XHash <QString, XVector<XVector3D> > &attributes3D() const;
    const XHash <QString, XVector<XVector4D> > &attributes4D() const;

    void setPoints( const XList<unsigned int> & );
    void setLines( const XList<unsigned int> & );
    void setTriangles( const XList<unsigned int> & );

    void setPoints( const XVector<unsigned int> & );
    void setLines( const XVector<unsigned int> & );
    void setTriangles( const XVector<unsigned int> & );

    void setLines(const xuint32 *, xsize size);
    void setTriangles(const xuint32 *, xsize size);

    void setAttribute( const QString &, const XList<xReal> & );
    void setAttribute( const QString &, const XList<XVector2D> & );
    void setAttribute( const QString &, const XList<XVector3D> & );
    void setAttribute( const QString &, const XList<XVector4D> & );

    void setAttribute( const QString &, const XVector<xReal> & );
    void setAttribute( const QString &, const XVector<XVector2D> & );
    void setAttribute( const QString &, const XVector<XVector3D> & );
    void setAttribute( const QString &, const XVector<XVector4D> & );

    void setAttribute( const QString &, const xReal *, xsize size );
    void setAttribute( const QString &, const XVector2D *, xsize size );
    void setAttribute( const QString &, const XVector3D *, xsize size );
    void setAttribute( const QString &, const XVector4D *, xsize size );

    void removeAttribute( const QString & );

    XCuboid computeBounds() const;

    void prepareInternal( XRenderer * ) const;
    XAbstractGeometry *internal() const;

    friend QDataStream EKS3D_EXPORT &operator<<( QDataStream &s, const XGeometry &geo );
    friend QDataStream EKS3D_EXPORT &operator>>( QDataStream &s, XGeometry &geo );

    bool operator==(const XGeometry &g) const
      {
      return _attr1 == g._attr1 &&
          _attr2 == g._attr2 &&
          _attr3 == g._attr3 &&
          _attr4 == g._attr4 &&
          _points == g._points &&
          _lines == g._lines &&
          _triangles == g._triangles;
      }

    bool operator!=(const XGeometry &g) const
      {
      return _attr1 != g._attr1 ||
          _attr2 != g._attr2 ||
          _attr3 != g._attr3 ||
          _attr4 != g._attr4 ||
          _points != g._points ||
          _lines != g._lines ||
          _triangles != g._triangles;
      }

private:
    mutable XAbstractGeometry *_internal;
    mutable XSet <QString> _changedA1;
    mutable XSet <QString> _changedA2;
    mutable XSet <QString> _changedA3;
    mutable XSet <QString> _changedA4;
    mutable bool _changedP;
    mutable bool _changedL;
    mutable bool _changedT;
    mutable bool _attributeSizeChanged;
    mutable bool _changedAttrs;
    int _attributeSize;
    XHash <QString, XVector<xReal> > _attr1;
    XHash <QString, XVector<XVector2D> > _attr2;
    XHash <QString, XVector<XVector3D> > _attr3;
    XHash <QString, XVector<XVector4D> > _attr4;
    XVector <unsigned int> _points;
    XVector <unsigned int> _lines;
    XVector <unsigned int> _triangles;
    mutable XRenderer *_renderer;

    BufferType _type;
    };

X_DECLARE_SERIALISABLE_METATYPE(XGeometry);
Q_DECLARE_METATYPE(XGeometry)

class EKS3D_EXPORT XAbstractGeometry
    {
public:
    virtual ~XAbstractGeometry();
    virtual void setPoints( const XVector<unsigned int> & ) = 0;
    virtual void setLines( const XVector<unsigned int> & ) = 0;
    virtual void setTriangles( const XVector<unsigned int> & ) = 0;

    virtual void setAttributesSize( int, int, int, int, int ) = 0;

    virtual void setAttribute( QString, const XVector<xReal> & ) = 0;
    virtual void setAttribute( QString, const XVector<XVector2D> & ) = 0;
    virtual void setAttribute( QString, const XVector<XVector3D> & ) = 0;
    virtual void setAttribute( QString, const XVector<XVector4D> & ) = 0;
    };

namespace XMeshUtilities
{
EKS3D_EXPORT bool intersect( QString semantic, const XLine &ray, const XGeometry &geo, XVector3DList *pos=0, XList <unsigned int> *tris=0 );
};

#endif // XGEOMETRY_H
