import "../EksBuild" as Eks;

Eks.Library {
    name: "Eks3D"
    toRoot: "../../"

    Export {
        Depends { name: "Qt.opengl" }
        cpp.includePaths: ["include"]
    }
}
