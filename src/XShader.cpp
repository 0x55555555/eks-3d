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

ShaderComponent::ShaderComponent(Renderer *r,
    ShaderType t,
    const char *source,
    xsize length,
    ParseErrorInterface *errors,
    const void *extra)
  {
  _renderer = 0;
  if(r)
    {
    delayedCreate(*this, r, t, source, length, errors, extra);
    }
  }

ShaderComponent::~ShaderComponent()
  {
  if(isValid())
    {
    _renderer->functions().destroy.shaderComponent(_renderer, this);
    }
  }

bool ShaderComponent::delayedCreate(ShaderComponent &ths,
    Renderer *r,
    ShaderType t,
    const char *source,
    xsize length,
    ParseErrorInterface *errors,
    const void *extraData)
  {
  xAssert(!ths.isValid());
  xAssert(r && source && length);
  ths._renderer = r;
  return r->functions().create.shaderComponent(r, &ths, t, source, length, errors, extraData);
  }

ShaderVertexComponent::ShaderVertexComponent(Renderer *r,
    const char *source,
    xsize length,
    const VertexLayout::Description *vertexDescription,
    xsize vertexItemCount,
    VertexLayout *layout,
    ParseErrorInterface *ifc)
  {
  _renderer = 0;
  if(r)
    {
    delayedCreate(*this, r, source, length, vertexDescription, vertexItemCount, layout, ifc);
    }
  }

bool ShaderVertexComponent::delayedCreate(ShaderVertexComponent &ths,
    Renderer *r,
    const char *source,
    xsize length,
    const VertexLayout::Description *vertexDescription,
    xsize vertexItemCount,
    VertexLayout *layout,
    ParseErrorInterface *ifc)
  {
  xAssert(!ths.isValid());
  xAssert(r && source && length);
  xAssert(!vertexDescription || (layout && vertexItemCount));
  ths._renderer = r;

  ExtraCreateData data = { vertexDescription, vertexItemCount, layout };

  bool result = r->functions().create.shaderComponent(
                  r,
                  &ths,
                  Vertex,
                  source,
                  length,
                  ifc,
                  &data);

  if(result && vertexDescription && layout)
    {
    layout->_renderer = r;
    }

  return result;
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
        ShaderComponent **v,
        xsize compCount,
        const char **outputs,
        xsize outputCount)
  {
  _renderer = 0;
  if(r)
    {
    delayedCreate(*this, r, v, compCount, outputs, outputCount);
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
    ShaderComponent **v,
    xsize cmpCount,
    const char **outputs,
    xsize outputCount)
  {
  xAssert(!ths.isValid());
  xAssert(r);
  ths._renderer = r;
  return r->functions().create.shader(r, &ths, v, cmpCount, outputs, outputCount);
  }
}
