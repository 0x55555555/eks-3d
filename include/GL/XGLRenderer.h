#ifndef XGLRENDERER_H
#define XGLRENDERER_H

#include "X3DGlobal.h"
#include "XRenderer.h"

namespace Eks
{

class AllocatorBase;
class XGLShader;
class XGLFramebuffer;

class EKS3D_EXPORT GLRenderer
  {
public:
  static Renderer *createGLRenderer(bool gles, Eks::AllocatorBase* alloc);
  static void destroyGLRenderer(Renderer *, Eks::AllocatorBase* alloc);
  };

}

#endif // XGLRENDERER_H
