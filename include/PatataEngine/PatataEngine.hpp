#pragma once

#include "PatataEngine/Libexport.hpp"

#include <cstdint>
#include <string>

#if defined(_MSC_VER)
#include <SDL.h>
#endif

namespace Patata
{
class Engine
{
public:
  PATATA_API Engine (const std::string &, const uint64_t &, const uint64_t &);
  PATATA_API ~Engine (void);

  PATATA_API void HandleEvent (SDL_Event & Event);
  PATATA_API void Render (void);

private:
  class EngineImpl;
  EngineImpl * pPatataEngine = nullptr;
};
}
