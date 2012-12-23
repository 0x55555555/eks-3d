#include "XShader.h"
#include "XRenderer.h"
#include "XTexture.h"

namespace Eks
{

ShaderVertexLayout::~ShaderVertexLayout()
  {
  if(isValid())
    {
    _renderer->destroyShaderVertexLayout(this);
    }
  }

ShaderVertexComponent::ShaderVertexComponent(
    Renderer *r,
    const char *source,
    xsize length,
    const VertexLayout::Description *vertexDescription,
    xsize vertexItemCount,
    VertexLayout *layout)
  {
  _renderer = 0;
  if(r)
    {
    delayedCreate(*this, r, source, length, vertexDescription, vertexItemCount, layout);
    }
  }

ShaderVertexComponent::~ShaderVertexComponent()
  {
  if(isValid())
    {
    _renderer->destroyVertexShaderComponent(this);
    }
  }

bool ShaderVertexComponent::delayedCreate(
    ShaderVertexComponent &ths,
    Renderer *r,
    const char *source,
    xsize length,
    const VertexLayout::Description *vertexDescription,
    xsize vertexItemCount,
    VertexLayout *layout)
  {
  xAssert(!ths.isValid());
  xAssert(r && source && length);
  xAssert(!vertexDescription || (layout && vertexItemCount));
  ths._renderer = r;
  bool result = r->createVertexShaderComponent(&ths,
                                               source,
                                               length,
                                               vertexDescription,
                                               vertexItemCount,
                                               layout);

  if(result && vertexDescription && layout)
    {
    layout->_renderer = r;
    }

  return result;
  }

ShaderFragmentComponent::ShaderFragmentComponent(Renderer *r, const char *source, xsize length)
  {
  _renderer = 0;
  if(r)
    {
    delayedCreate(*this, r, source, length);
    }
  }

ShaderFragmentComponent::~ShaderFragmentComponent()
  {
  if(isValid())
    {
    _renderer->destroyFragmentShaderComponent(this);
    }
  }

bool ShaderFragmentComponent::delayedCreate(ShaderFragmentComponent &ths,
                                           Renderer *r,
                                           const char *source,
                                           xsize length)
  {
  xAssert(!ths.isValid());
  xAssert(r && source && length);
  ths._renderer = r;
  return r->createFragmentShaderComponent(&ths, source, length);
  }

Shader::Shader(Renderer *r,
        ShaderVertexComponent *v,
        ShaderFragmentComponent *f)
  {
  _renderer = 0;
  if(r)
    {
    delayedCreate(*this, r, v, f);
    }
  }

Shader::~Shader()
  {
  if(isValid())
    {
    _renderer->destroyShader(this);
    }
  }

bool Shader::delayedCreate(Shader &ths, Renderer *r, ShaderVertexComponent *v, ShaderFragmentComponent *f)
  {
  xAssert(!ths.isValid());
  xAssert(r);
  ths._renderer = r;
  return r->createShader(&ths, v, f);
  }

}
