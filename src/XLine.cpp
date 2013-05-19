#include "XLine.h"
#include "XPlane.h"

namespace Eks
{

float Line::closestPointOn(const Line &l) const
  {
  auto lDirNorm = l.direction().normalized();
  auto dirNorm = direction().normalized();

  auto cP = lDirNorm.cross( lDirNorm.cross( dirNorm ) );
  if(cP.squaredNorm() > 0.001f)
    {
    return Plane( l.position(), cP ).intersection(*this);
    }

  return HUGE_VAL;
  }

}
