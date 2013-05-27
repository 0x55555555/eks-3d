#ifndef XGLRENDERER_H
#define XGLRENDERER_H

#include "X3DGlobal.h"

#if X_ENABLE_GL_RENDERER

#include "XRenderer.h"
#include "QSize"

class QGLContext;

namespace Eks
{

class AllocatorBase;
class XGLShader;
class XGLFramebuffer;

class EKS3D_EXPORT GLRenderer
  {
public:
  static Renderer *createGLRenderer(ScreenFrameBuffer *buffer, Eks::AllocatorBase* alloc);
  static void destroyGLRenderer(Renderer *, ScreenFrameBuffer *buffer, Eks::AllocatorBase* alloc);
  };

}

#endif

#endif // XGLRENDERER_H
