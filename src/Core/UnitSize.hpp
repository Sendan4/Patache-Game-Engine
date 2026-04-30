#pragma once

#include <cstring>
#include <cassert>

#include "CstringWrapped.hpp"

namespace Patache
{
inline void
CalculateSizeUnit (float & rUnit, char * pSizeUnitName, std::uint32_t strSize)
{
  assert ("Patache::CalculeSizeUnit(), pSizeUnitName cannot be nullptr"
          && pSizeUnitName != nullptr);

  if (rUnit >= 1073741824U)
    {
      PATACHE_STRNCPY (pSizeUnitName, "GiB", strSize, strSize);
      rUnit = rUnit / 1073741824.0F;
      return;
    }

  if (rUnit >= 1048576U)
    {
      PATACHE_STRNCPY (pSizeUnitName, "MiB", strSize, strSize);
      rUnit = rUnit / 1048576.0F;
      return;
    }

  if (rUnit >= 1024U)
    {
      PATACHE_STRNCPY (pSizeUnitName, "KiB", strSize, strSize);
      rUnit = rUnit / 1024.0F;
    }
}
}
