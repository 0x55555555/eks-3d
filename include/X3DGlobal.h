#ifndef X3DGLOBAL_H
#define X3DGLOBAL_H

#include "XGlobal.h"

#ifndef EKS3D_STATIC
# if defined(EKS3D_BUILD)
#  define EKS3D_EXPORT X_DECL_EXPORT
# else
#  define EKS3D_EXPORT X_DECL_IMPORT
# endif
#else
# define EKS3D_EXPORT
#endif

#if EKS_XREAL_IS_DOUBLE == 0
#define EKS_XREAL_GL_TYPE GL_FLOAT
#else
#define EKS_XREAL_GL_TYPE GL_DOUBLE
#endif

namespace Eks
{
enum TextureFormat
  {
  Rgba8,
  Depth24,

  TextureFormatCount
  };
};

#endif // X3DGLOBAL_H
