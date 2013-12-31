import "../EksBuild" as Eks;

Eks.Library {
  property bool windows: qbs.targetOS == "windows"
  property bool linux: qbs.targetOS == "linux"
  property bool osx: !windows && !linux

  property bool angle: Qt.core.qtConfig.contains("angle")
  property string engine: windows ? angle ? "Opengl" : "GLES" : "Opengl" // [ "Opengl", "GLES", "D3D" ]
  name: "Eks3D"
  toRoot: "../../"

  Depends { name: "EksCore" }
  Depends { name: "Qt.core" }
  Depends {
    name: "Qt"
    submodules: [ "gui", "opengl", "widgets" ]
  }


  cpp.includePaths: base.concat( [ "3rdParty" ] )

  Group {
    name: "OpenGL"
    condition: engine == "Opengl"

    files: [ "include/XGL*", "src/XGL*", "3rdParty/GL/*" ]
  }
  Properties {
    condition: engine == "Opengl"

    cpp.defines: base.concat( [ "GLEW_STATIC", "X_ENABLE_GL_RENDERER" ] )
  }

  Properties {
    condition: windows && engine == "Opengl"
    cpp.dynamicLibraries: [ "OpenGL32", "Gdi32", "User32" ]
  }

  Properties {
    condition: osx && engine == "Opengl"
    cpp.frameworks: [ "OpenGL" ]
  }


  Group {
    name: "GLES"
    condition: engine == "GLES"

    files: [ "include/XGL*", "src/XGL*" ]
  }
  Properties {
    condition: engine == "GLES"
    cpp.libraryPaths: base.concat( [ Qt.core.libPath ] )
    cpp.dynamicLibraries: [ "libGLESv2d", "libEGLd", "Gdi32", "User32" ]

    cpp.defines: base.concat( [ "GLEW_STATIC", "X_GLES", "X_ENABLE_GL_RENDERER" ] )
  }

  Group {
    name: "D3D"
    condition: engine == "D3D"

    files: [ "include/XD3D*", "src/XD3D*" ]
  }
  Properties {
    condition: engine == "D3D"
    cpp.dynamicLibraries: [ "d2d1", "d3d11", "dxgi", "windowscodecs", "dwrite" ]

    cpp.defines: base.concat( [ "X_ENABLE_DX_RENDERER" ] )
  }

  Group {
    name: "Other"

    files: [ "include/*", "src/*" ]

    excludeFiles: [ "include/XGL*", "include/XD3D*", "src/XGL*", "src/XD3D*" ]
  }

  Group {
    name: "Examples"

    files: [ "examples/**/*" ]
  }

  Export {
    Depends { name: "cpp" }
    cpp.includePaths: ["include"]
  }
}
