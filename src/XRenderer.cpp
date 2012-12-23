#include "XRenderer.h"

namespace Eks
{

Renderer::Renderer( ) : _renderFlags(0)
  {
  }

Renderer::~Renderer( )
  {
  }

void Renderer::setRenderFlags( int flags )
  {
  for( unsigned int x=0; x<sizeof(int)*8; x++ )
    {
    int flag = 1 << x;
    bool newHasFlag = (flags & flag) != 0;

    if(newHasFlag != _renderFlags.hasAnyFlags(flag))
      {
      if(newHasFlag)
        {
        enableRenderFlag( (RenderFlags)(1 << x) );
        }
      else
        {
        disableRenderFlag( (RenderFlags)(1 << x) );
        }
      }
    }
  _renderFlags = flags;
  }

int Renderer::renderFlags() const
  {
  return *_renderFlags;
  }

}
