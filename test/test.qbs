import "../../../Eks/EksBuild" as Eks;

Eks.Test {
  name: "Eks3DTest"
  toRoot: "../../"
  
  Depends { name: "Eks3D" }
}
