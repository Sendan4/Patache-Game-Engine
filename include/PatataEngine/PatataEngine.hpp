#pragma once

#include <cstdint>
#include <string>

#include "PatataEngine/Libexport.hpp"
#include "PatataEngine/Color.hpp"

namespace Patata
{
struct ClearColor {
  float  r = 0.0f;
  float  g = 0.0f;
  float  b = 0.0f;
  float  a = 1.0f;
};

class Engine
{
public:
  PATATA_API Engine (const std::string &);
  PATATA_API ~Engine (void);

  PATATA_API void HandleEvent (SDL_Event &); // Patata Events
  PATATA_API bool BeginRender (SDL_Event &);
  PATATA_API void EndRender (SDL_Event &);

  // Clear Color
  Patata::ClearColor * pClearColor = nullptr;
  PATATA_API void      ClearColor (const eColor &);
  PATATA_API void      ClearColorRGBA (const float &, const float &, const float &,
                                  const float &);
  PATATA_API void ClearColorRGBA (const float &, const float &, const float &);

private:
  class EngineImpl;
  EngineImpl * pPatataEngine = nullptr;
}; // End Engine Namespace
} // End Patata Namespace
