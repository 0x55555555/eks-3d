import "../EksBuild" as Eks;

Eks.Library {
  property string engine: "Opengl" // [ "Opengl", "D3D" ]
  name: "Eks3D"
  toRoot: "../../"

  Depends { name: "EksCore" }
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
    cpp.dynamicLibraries: [ "OpenGL32", "Gdi32", "User32" ]

    cpp.defines: base.concat( [ "GLEW_STATIC", "X_ENABLE_GL_RENDERER" ] )
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
      //cpp.includePaths: ["include"]
  }
}
