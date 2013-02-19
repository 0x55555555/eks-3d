#ifndef X3DGLOBAL_H
#define X3DGLOBAL_H

#include "XGlobal"

#ifndef EKS3D_STATIC
# if defined(EKS3D_BUILD)
#  define EKS3D_EXPORT Q_DECL_EXPORT
# else
#  define EKS3D_EXPORT Q_DECL_IMPORT
# endif
#else
# define EKS3D_EXPORT
#endif

#if EKS_XREAL_IS_DOUBLE == 0
#define EKS_XREAL_GL_TYPE GL_FLOAT
#else
#define EKS_XREAL_GL_TYPE GL_DOUBLE
#endif

#include "XProfiler"

#define X3DDataModelProfileScope 496
#define X3DDataModelFunction XProfileFunctionBase(X3DDataModelProfileScope)
#define X3DDataModelFunctionScopedBlock(mess) XProfileScopedBlockBase(X3DDataModelProfileScope, mess)

#ifdef Q_CC_MSVC
# define X_ENABLE_GL_RENDERER 0
# define X_ENABLE_DX_RENDERER 1
#else
# define X_ENABLE_GL_RENDERER 1
# define X_ENABLE_DX_RENDERER 0
#endif

#endif // X3DGLOBAL_H
