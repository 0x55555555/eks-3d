#include "XColladaFile.h"
#include "XGeometry.h"
#include "QFile"
#include "QDebug"

XList<int> getListInt( QString data )
    {
    QTextStream stream( &data, QIODevice::ReadOnly );
    XList<int> ret;

    int oldPos( -1 );
    while( !stream.atEnd() && oldPos != stream.pos() )
        {
        oldPos = stream.pos();

        int x;
        stream >> x;
        ret << x;
        }
    return ret;
    }

XList<xReal> getList1D( QString data, int count )
    {
    QTextStream stream( &data, QIODevice::ReadOnly );
    XList<xReal> ret;

    for( int i=0; i<count; i++ )
        {
        float x;
        stream >> x;
        ret << x;
        }
    return ret;
    }

XList<XVector2D> getList2D( QString data, int count )
    {
    QTextStream stream( &data, QIODevice::ReadOnly );
    XList<XVector2D> ret;

    for( int i=0; i<count; i++ )
        {
        float x, y;
        stream >> x >> y;
        ret << XVector2D( x, y );
        }
    return ret;
    }

XList<XVector3D> getList3D( QString data, int count )
    {
    QTextStream stream( &data, QIODevice::ReadOnly );
    XList<XVector3D> ret;

    for( int i=0; i<count; i++ )
        {
        float x, y, z;
        stream >> x >> y >> z;
        ret << XVector3D( x, y, z );
        }
    return ret;
    }

XList<XVector4D> getList4D( QString data, int count )
    {
    QTextStream stream( &data, QIODevice::ReadOnly );
    XList<XVector4D> ret;

    for( int i=0; i<count; i++ )
        {
        float x, y, z, w;
        stream >> x >> y >> z >> w;
        ret << XVector4D( x, y, z, w );
        }
    return ret;
    }

XColladaFile::XColladaFile( QString name ) : _document( "COLLADA" )
    {
    QFile f( name );
    if( !f.open( QFile::ReadOnly ) )
        {
        return;
        }
    _document.setContent( f.readAll() );

    QDomElement docElement( _document.documentElement() );

    int num = 0;
    if( !docElement.isNull() && docElement.tagName() == "COLLADA" )
        {
        QDomElement colladaElement = docElement.firstChildElement("library_geometries");
        if( !colladaElement.isNull() )
            {
            QDomElement geometryElement = colladaElement.firstChildElement("geometry");
            while( !geometryElement.isNull() )
                {
                QString str( geometryElement.attribute("name") );
                if( str == "" )
                    {
                    str = QString::number( num );
                    }

                QDomElement meshElement( geometryElement.firstChildElement() );
                if( !meshElement.isNull() && meshElement.tagName() == "mesh" )
                    {
                    _geometries.insert( str, meshElement );
                    }

                num++;
                geometryElement = geometryElement.nextSiblingElement("geometry");
                }
            }
        }
    }

bool XColladaFile::geometryExists( QString in )
    {
    return _geometries.contains( in );
    }

QStringList XColladaFile::geometryNames() const
    {
    return _geometries.keys();
    }

XGeometry XColladaFile::geometry( QString name ) const
    {
    XGeometry ret;

    XHash <QString, XList<xReal> > attr1D;
    XHash <QString, XList<XVector2D> > attr2D;
    XHash <QString, XList<XVector3D> > attr3D;
    XHash <QString, XList<XVector4D> > attr4D;

    if( !_geometries.contains( name ) )
        {
        return ret;
        }

    QDomElement mesh( _geometries[name] );

    QDomElement sourceElement( mesh.firstChildElement("source") );
    while( !sourceElement.isNull() )
        {
        QString name( sourceElement.attribute("id") );

        QDomElement arrayElement( sourceElement.firstChildElement("float_array") );

        int count( arrayElement.attribute("count").toInt() );

        int size = 0;

        // Analyse Array Contents
            {
            QDomElement techniqueElement( sourceElement.firstChildElement("technique_common") );
            if( !techniqueElement.isNull() )
                {
                QDomElement accessorElement( techniqueElement.firstChildElement("accessor") );
                if( !accessorElement.isNull() )
                    {

                    QDomElement paramElement( accessorElement.firstChildElement("param") );
                    while( !paramElement.isNull() )
                        {
                        paramElement = paramElement.nextSiblingElement("param");
                        size++;
                        }
                    }
                }
            }

        xAssert( ( count % size ) == 0 );

        if( size == 1 )
            {
            XList<xReal> floats( getList1D( arrayElement.text(), count/size ) );
            attr1D.insert( name, floats );
            }
        else if( size == 2 )
            {
            XList<XVector2D> floats( getList2D( arrayElement.text(), count/size ) );
            attr2D.insert( name, floats );
            }
        else if( size == 3 )
            {
            XList<XVector3D> floats( getList3D( arrayElement.text(), count/size ) );
            attr3D.insert( name, floats );
            }
        else if( size == 4 )
            {
            XList<XVector4D> floats( getList4D( arrayElement.text(), count/size ) );
            attr4D.insert( name, floats );
            }

        sourceElement = sourceElement.nextSiblingElement("source");
        }

    unsigned int inputNum = 0;
    unsigned int maxOffset = 0;

    int posOff = -1;
    XList<XVector3D> *posPtr = 0;
    int norOff = -1;
    XList<XVector3D> *norPtr = 0;
    int texOff = -1;
    XList<XVector2D> *texPtr = 0;

    QDomElement inputParent;

    // could search for different polygon sections here
    if( !mesh.firstChildElement("polylist").isNull() )
        {
        inputParent = mesh.firstChildElement("polylist");
        }
    else if( !mesh.firstChildElement("triangles").isNull() )
        {
        inputParent = mesh.firstChildElement("triangles");
        }
    else
        {
        inputParent = mesh.firstChildElement("polygons");
        }

    QDomElement inputElement = inputParent.firstChildElement("input");
    while( !inputElement.isNull() )
        {
        QString semantic = inputElement.attribute("semantic");
        unsigned int offset = inputElement.attribute("offset").toUInt();
        QString location;

        if( offset > maxOffset )
            {
            maxOffset = offset;
            }

        if( semantic == "VERTEX" )
            {
            QDomElement vertivesElement( mesh.firstChildElement("vertices").firstChildElement("input") );
            xAssert( vertivesElement.attribute("semantic") == "POSITION" );
            semantic = "POSITION";
            location = vertivesElement.attribute("source");
            }
        else
            {
            location = inputElement.attribute("source");
            }

        location = location.mid(1);

        if( semantic == "POSITION" )
            {
            foreach( QString name, attr3D.keys() )
                {
                if( name == location )
                    {
                    posPtr = &(attr3D[name]);
                    posOff = offset;
                    }
                }
            }
        if( semantic == "NORMAL" )
            {
            foreach( QString name, attr3D.keys() )
                {
                if( name == location )
                    {
                    norPtr = &(attr3D[name]);
                    norOff = offset;
                    }
                }
            }
        if( semantic == "TEXCOORD" )
            {
            foreach( QString name, attr2D.keys() )
                {
                if( name == location )
                    {
                    texPtr = &(attr2D[name]);
                    texOff = offset;
                    }
                }
            }

        ++inputNum;
        inputElement = inputElement.nextSiblingElement("input");
        }

    xAssert( posOff != -1 );

    XList <XVector3D> positions;
    XList <XVector3D> normals;
    XList <XVector2D> textures;

    XList <unsigned int> indices;

    int vertexSize = maxOffset + 1;

    if( !mesh.firstChildElement("triangles").isNull() )
        {
        QDomElement triangles = mesh.firstChildElement("triangles");
        unsigned int triangleSize = 3 * vertexSize;

        XList <int> triIndexes = getListInt( triangles.firstChildElement("p").text() );
        xAssert( ( triIndexes.size()%triangleSize ) == 0 );

        unsigned int triNum = 0;
        for( int x=0; x<triIndexes.size(); x+=triangleSize )
            {
            for( unsigned int y=0; y<3; y++ )
                {
                indices << positions.size();
                positions << (*posPtr)[triIndexes[x+(y*vertexSize)+posOff]];
                if( norPtr )
                    {
                    normals << (*norPtr)[triIndexes[x+(y*vertexSize)+norOff]];
                    }
                if( texPtr )
                    {
                    textures << (*texPtr)[triIndexes[x+(y*vertexSize)+texOff]];
                    }
                }
            triNum++;
            }
        }
    else if( !mesh.firstChildElement("polylist").isNull() )
        {
        QDomElement polygons = mesh.firstChildElement("polylist");

        QDomElement pE = polygons.firstChildElement("p");
        QDomElement vcount = polygons.firstChildElement("vcount");
        if( !pE.isNull() && !vcount.isNull() )
            {
            XList <int> polySizes = getListInt( vcount.text() );
            XList <int> polyIndices = getListInt( pE.text() );

            int startPoly=0;

            for( int x=0; x<polySizes.size(); x++ )
                {
                int firstVertex = positions.size();

                for( int y=0; y<polySizes[x]; y++ )
                    {
                    positions << (*posPtr)[polyIndices[startPoly]];
                    if( norPtr )
                        {
                        normals << (*norPtr)[polyIndices[startPoly+1]];
                        }
                    if( texPtr )
                        {
                        textures << (*texPtr)[polyIndices[startPoly+2]];
                        }
                    startPoly += vertexSize;
                    }

                for( int y=0; y<polySizes[x]-2; y++ )
                    {
                    indices << firstVertex << firstVertex+y+1 << firstVertex+y+2;
                    }
                }
            }

            pE = pE.nextSiblingElement("p");
        }
    else
        {
        QDomElement polygons = mesh.firstChildElement("polygons");

        QDomElement pE = polygons.firstChildElement("p");
        while( !pE.isNull() )
            {
            XList <int> polyIndices = getListInt( pE.text() );

            int firstVertex = positions.size();
            for( int x=0; x<polyIndices.size(); x+=vertexSize )
                {
                positions << (*posPtr)[polyIndices[x]];
                if( norPtr )
                    {
                    normals << (*norPtr)[polyIndices[x+1]];
                    }
                if( texPtr )
                    {
                    textures << (*texPtr)[polyIndices[x+2]];
                    }
                }

            for( int x=0; x<(polyIndices.size()/vertexSize)-2; x++ )
                {
                indices << firstVertex << firstVertex+x+1 << firstVertex+x+2;
                }

            pE = pE.nextSiblingElement("p");
            }
        }

    ret.setAttribute( "vertex", positions );
    ret.setAttribute( "normal", normals );
    ret.setAttribute( "texture", textures );

    ret.setTriangles( indices );

    return ret;
    }
