#ifndef XABSTRACTSHADER_H
#define XABSTRACTSHADER_H

#include "X3DGlobal.h"
#include "XPrivateImpl"

class XShader;
class XRenderer;

class EKS3D_EXPORT XShaderVertexComponent : public XPrivateImpl<sizeof(void*)>
  {
public:
  XShaderVertexComponent(XRenderer *r=0,
                         const char *source=0,
                         xsize length=0);
  ~XShaderVertexComponent();

  static bool delayedCreate(XShaderVertexComponent &ths,
                            XRenderer *r,
                            const char *source,
                            xsize length);

private:
  X_DISABLE_COPY(XShaderVertexComponent);

  XRenderer *_renderer;
  };

class EKS3D_EXPORT XShaderFragmentComponent : public XPrivateImpl<sizeof(void*)>
  {
public:
  XShaderFragmentComponent(XRenderer *r=0,
                           const char *source=0,
                           xsize length=0);
  ~XShaderFragmentComponent();

  static bool delayedCreate(XShaderFragmentComponent &ths,
                            XRenderer *r,
                            const char *source,
                            xsize length);

private:
  X_DISABLE_COPY(XShaderFragmentComponent);

  XRenderer *_renderer;
  };

class EKS3D_EXPORT XShader : public XPrivateImpl<sizeof(void*)*2>
  {
public:
  XShader(XRenderer *r=0,
          XShaderVertexComponent *v=0,
          XShaderFragmentComponent *f=0);
  ~XShader();

  static bool delayedCreate(XShader &ths,
              XRenderer *r,
              XShaderVertexComponent *v,
              XShaderFragmentComponent *f);

private:
  X_DISABLE_COPY(XShader);

  XRenderer *_renderer;
  };

#endif // XABSTRACTSHADER_H
