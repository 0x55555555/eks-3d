#ifndef XGLRENDERER_H
#define XGLRENDERER_H

#include "XGlobal"

#ifndef X_ARCH_ARM

#include "XRenderer.h"
#include "QSize"

class QGLContext;
class XGLShader;
class XGLFramebuffer;

class EKS3D_EXPORT XGLRenderer : public XRenderer
    {
public:
    XGLRenderer();

    void setContext(QGLContext *ctx);

    QGLContext *context();
    const QGLContext *context() const;

    void intialise();

    void setViewportSize( QSize );

    bool createGeometry(
      XGeometry *g,
      const void *data,
      xsize elementSize,
        xsize elementCount) X_OVERRIDE;

    bool createShader(
      XShader *s,
      XShaderVertexComponent *v,
      XShaderFragmentComponent *f) X_OVERRIDE;

    XAbstractTexture *getTexture() X_OVERRIDE;
    XAbstractFramebuffer *getFramebuffer( int options, int cf, int df, int width, int heightg ) X_OVERRIDE;

    void debugRenderLocator(DebugLocatorMode m) X_OVERRIDE;

    void destroyTexture( XAbstractTexture * ) X_OVERRIDE;
    void destroyFramebuffer( XAbstractFramebuffer * ) X_OVERRIDE;

    void pushTransform( const XTransform & ) X_OVERRIDE;
    void popTransform( ) X_OVERRIDE;

    void setClearColour(const XColour &col) X_OVERRIDE;
    void clear(int=ClearColour|ClearDepth) X_OVERRIDE;

    void enableRenderFlag( RenderFlags ) X_OVERRIDE;
    void disableRenderFlag( RenderFlags ) X_OVERRIDE;

    void setProjectionTransform( const XComplexTransform & ) X_OVERRIDE;

    void setShader( const XShader * ) X_OVERRIDE;

    void drawTriangles(const XIndexGeometry *indices, const XGeometry *vert) X_OVERRIDE;

    void setFramebuffer( const XFramebuffer * ) X_OVERRIDE;

    QSize viewportSize() X_OVERRIDE;

    QGLContext *_context;
    XShader *_currentShader;
    XShaderVertexLayout *_vertexLayout;
    QSize _size;
    XGLFramebuffer *_currentFramebuffer;
    XVector <int> m_ids;
    };

#endif

#endif // XGLRENDERER_H
