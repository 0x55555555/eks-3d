#include "XShader.h"
#include "XRenderer.h"
#include "XTexture.h"

namespace Eks
{

ShaderVertexLayout::~ShaderVertexLayout()
  {
  if(isValid())
    {
    _renderer->functions().destroy.shaderVertexLayout(_renderer, this);
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
    _renderer->functions().destroy.vertexShaderComponent(_renderer, this);
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
  bool result = r->functions().create.vertexShaderComponent(
                  r,
                  &ths,
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
    _renderer->functions().destroy.fragmentShaderComponent(_renderer, this);
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
  return r->functions().create.fragmentShaderComponent(r, &ths, source, length);
  }

ShaderConstantData::ShaderConstantData(
    Renderer *r,
    Description *desc,
    xsize descCount,
    const void *data)
   : _renderer(0)
  {
  if(r)
    {
    delayedCreate(*this, r, desc, descCount, data);
    }
  }

ShaderConstantData::~ShaderConstantData()
  {
  if(_renderer)
    {
    _renderer->functions().destroy.shaderConstantData(_renderer, this);
    }
  }

bool ShaderConstantData::delayedCreate(
    ShaderConstantData &ths,
    Renderer *r,
    Description *desc,
    xsize descCount,
    const void *data)
  {
  xAssert(!ths.isValid());
  xAssert(r);
  xAssert(desc && descCount);
  ths._renderer = r;
  return r->functions().create.shaderConstantData(r, &ths, desc, descCount, data);
  }

Shader::Shader(Renderer *r,
        ShaderVertexComponent *v,
        ShaderFragmentComponent *f,
        const char **outputs,
        xsize outputCount)
  {
  _renderer = 0;
  if(r)
    {
    delayedCreate(*this, r, v, f, outputs, outputCount);
    }
  }

Shader::~Shader()
  {
  if(isValid())
    {
    _renderer->functions().destroy.shader(_renderer, this);
    }
  }

bool Shader::delayedCreate(
    Shader &ths,
    Renderer *r,
    ShaderVertexComponent *v,
    ShaderFragmentComponent *f,
    const char **outputs,
    xsize outputCount)
  {
  xAssert(!ths.isValid());
  xAssert(r);
  ths._renderer = r;
  return r->functions().create.shader(r, &ths, v, f, outputs, outputCount);
  }
}
