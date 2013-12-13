# -------------------------------------------------
# Project created by QtCreator 2010-04-21T15:51:26
# -------------------------------------------------

TARGET = Eks3D
TEMPLATE = lib

QT += widgets

DEFINES += X_GLES

include("../../EksCore/GeneralOptions.pri")

SOURCES += \
    ../src/XTransform.cpp \
    ../src/XGeometry.cpp \
    ../src/XShader.cpp \
    ../src/XTexture.cpp \
    ../src/XModeller.cpp \
    ../src/XColladaFile.cpp \
    ../src/XFrustum.cpp \
    ../src/XPlane.cpp \
    ../src/XTriangle.cpp \
    ../src/XAbstractCanvas.cpp \
    ../src/X2DCanvas.cpp \
    ../src/XAbstractRenderModel.cpp \
    ../src/XAbstractCanvasController.cpp \
    ../src/XCameraCanvasController.cpp \
    ../src/XObjLoader.cpp \
    ../src/XRasteriserState.cpp \
    ../src/XFrameBuffer.cpp \
    ../src/X3DCanvas.cpp \
    ../src/XDepthStencilState.cpp \
    ../src/XBlendState.cpp \
    ../src/XLine.cpp \
    ../src/XBoundingBox.cpp

HEADERS += \
    ../include/X3DGlobal.h \
    ../include/XTransform.h \
    ../include/XRenderer.h \
    ../include/XGeometry.h \
    ../include/XShader.h \
    ../include/XTexture.h \
    ../include/XModeller.h \
    ../include/XColladaFile.h \
    ../include/XFrustum.h \
    ../include/XPlane.h \
    ../include/XLine.h \
    ../include/XTriangle.h \
    ../include/XAbstractCanvas.h \
    ../include/X2DCanvas.h \
    ../include/XAbstractRenderModel.h \
    ../include/XAbstractDelegate.h \
    ../include/XAbstractCanvasController.h \
    ../include/XCameraCanvasController.h \
    ../include/XObjLoader.h \
    ../include/XRasteriserState.h \
    ../include/XFrameBuffer.h \
    ../include/X3DCanvas.h \
    ../include/XDepthStencilState.h \
    ../include/XBlendState.h \
    ../3rdParty/GL/wglew.h \
    ../3rdParty/GL/glxew.h \
    ../3rdParty/GL/glew.h \
    ../examples/Simple3DExample.h \
    ../include/XBoundingBox.h

INCLUDEPATH += ../include/ \
    $$ROOT/Eks/EksCore/include/

win32-arm-msvc2012|win32-msvc2012 {
  SOURCES += ../src/XD3DRenderer.cpp \
    ../src/XD3DRendererImpl.cpp

  DEFINES += EKSCORE_STATIC X_ENABLE_DX_RENDERER

  HEADERS += ../include/XD3DRenderer.h \
    ../include/XD3DRendererImpl.h

  LIBS += -ld2d1 -ld3d11 -ldxgi -lwindowscodecs -ldwrite
}

!win32-arm-msvc2012 {
  QT += opengl \
    xml

  win32 {
    LIBS += -lOpenGL32
  }

  INCLUDEPATH += ../3rdParty
  DEFINES += GLEW_STATIC
  DEFINES += X_ENABLE_GL_RENDERER
  SOURCES += ../3rdParty/GL/glew.c \
             ../src/XGLRenderer.cpp

  HEADERS += ../include/XGLRenderer.h

  RESOURCES += \
    ../GLResources.qrc

  OTHER_FILES += \
    ../GLResources/shaders/default.vert \
    ../GLResources/shaders/default.frag \
    ../GLResources/shaders/blinn.vert \
    ../GLResources/shaders/blinn.frag \
    ../GLResources/shaders/plainColour.frag \
    ../GLResources/shaders/plainColour.vert \
    ../GLResources/shaders/standardSurface.frag \
    ../GLResources/shaders/standardSurface.vert

}

LIBS += -lEksCore
















