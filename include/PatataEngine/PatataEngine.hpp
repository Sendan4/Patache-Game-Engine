#pragma once

#include "PatataEngine/Libexport.hpp"
#include "PatataEngine/Color.hpp"
#include "PatataEngine/StructClearColor.hpp"

namespace Patata
{
class Engine
{
public:
  /*
   * The argument of this constructor is used to name the window.
   * The argument of this constructor overrides GAME_NAME defined as a macro in
   * CMake and Meson.
   */
  PATATA_API Engine (const char *);
  /*
   * If you prefer, you can start without assigning a name to the window.
   * If you use this constructor and also use GAME_NAME in CMake or Meson,
   * whatever you wrote will be the name of your window.
   */
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
