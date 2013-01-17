#ifndef XGLRENDERER_H
#define XGLRENDERER_H

#include "X3DGlobal.h"

#if X_ENABLE_GL_RENDERER

#include "XRenderer.h"
#include "QSize"

class QGLContext;

namespace Eks
{

class XGLShader;
class XGLFramebuffer;

class EKS3D_EXPORT GLRenderer : public Renderer
  {
public:
  static Renderer *createGLRenderer(ScreenFrameBuffer *buffer);
  static void destroyGLRenderer(Renderer *, ScreenFrameBuffer *buffer);

private:
  QGLContext *_context;
  Shader *_currentShader;
  ShaderVertexLayout *_vertexLayout;
  QSize _size;
  XGLFramebuffer *_currentFramebuffer;
  };

}

#endif

#endif // XGLRENDERER_H
