#pragma once

#include <cstdint>

#include "PatataEngine/Libexport.hpp"
#include "PatataEngine/Color.hpp"
#include "PatataEngine/StructClearColor.hpp"

namespace Patata
{
class Engine
{
public:
  PATATA_API Engine (const char *);
  PATATA_API Engine (void);
  PATATA_API ~Engine (void);

  PATATA_API void HandleEvent (SDL_Event &); // Patata Events
  PATATA_API bool BeginRender (SDL_Event &);
  PATATA_API void EndRender (SDL_Event &);

  // Clear Color
  Patata::ClearColor * pClearColor = nullptr;
  PATATA_API void      ClearColor (const eColor &);
  PATATA_API void ClearColorRGBA (const float &, const float &, const float &,
                                  const float &);
  PATATA_API void ClearColorRGBA (const float &, const float &, const float &);

private:
  class EngineImpl;
  EngineImpl * pPatataEngine = nullptr;
}; // End Engine Namespace
} // End Patata Namespace
