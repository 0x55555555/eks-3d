import "../../../Eks/EksBuild" as Eks;

Eks.Application {
  name: "Eks3DExample"
  toRoot: "../../"

  files: [ "**" ]

  Depends { name: "EksGui" }
  Depends { name: "Eks3D" }
  Depends { name: "EksCore" }

  Depends {
    name: "Qt"
    submodules: [ "gui", "widgets", "opengl" ]
  }
}
