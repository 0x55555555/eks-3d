#ifndef XCOLLADAFILE_H
#define XCOLLADAFILE_H

#if 0

#include "X3DGlobal.h"
#include "QDomDocument"
#include "XHash"
#include "QStringList"

class Geometry;
class QDomElement;

class EKS3D_EXPORT XColladaFile
    {
public:
    XColladaFile( QString );
    
    bool geometryExists( QString );
    QStringList geometryNames() const;
    Geometry geometry( QString ) const;

private:
    XHash <QString, QDomElement> _geometries;
    QDomDocument _document;
    };

#endif

#endif // XCOLLADA_H
