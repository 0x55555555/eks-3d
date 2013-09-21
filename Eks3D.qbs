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

  cpp.defines: base.concat( [ "GLEW_STATIC" ] )
  cpp.includePaths: base.concat( [ "3rdParty" ] )

  Group {
    name: "OpenGL"
    condition: engine == "Opengl"

    files: [ "include/XGL*", "src/XGL*", "3rdParty/GL/*" ]

    cpp.dynamicLibraries: [ "OpenGL32" ]
  }

  Group {
    name: "D3D"
    condition: engine == "d3d"

    files: [ "include/XD3D*", "src/XD3D*" ]

    cpp.dynamicLibraries: [ "d2d1", "d3d11", "dxgi", "windowscodecs", "dwrite" ]
  }

  Group {
    name: "Other"

    files: [ "include/*", "src/*" ]

    excludeFiles: [ "include/XGL*", "include/XD3D*", "src/XGL*", "src/XD3D*" ]
  }

  Export {
      //cpp.includePaths: ["include"]
  }
}
