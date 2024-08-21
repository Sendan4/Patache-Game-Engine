#pragma once

#include "PatataEngine/Libexport.hpp"

#include <cstdint>
#include <string>

namespace Patata
{
class Engine
{
public:
  PATATA_API Engine (const std::string &, const uint32_t &, const uint32_t &);
  PATATA_API ~Engine (void);

  PATATA_API void HandleEvent (SDL_Event & Event); // Patata Events
  PATATA_API void BeginRender (void);
  PATATA_API void EndRender (void);
  PATATA_API void ClearColor (const float &, const float &, const float &, const float &); // R G B A

private:
  class EngineImpl;
  EngineImpl * pPatataEngine = nullptr;
};
}
