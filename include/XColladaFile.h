#ifndef XCOLLADAFILE_H
#define XCOLLADAFILE_H

#include "X3DGlobal.h"
#include "QDomDocument"
#include "XHash"
#include "QStringList"

class XGeometry;
class QDomElement;

class EKS3D_EXPORT XColladaFile
    {
public:
    XColladaFile( QString );
    
    bool geometryExists( QString );
    QStringList geometryNames() const;
    XGeometry geometry( QString ) const;

private:
    XHash <QString, QDomElement> _geometries;
    QDomDocument _document;
    };

#endif // XCOLLADA_H
