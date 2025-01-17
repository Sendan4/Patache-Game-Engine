#pragma once

namespace Patata
{
struct Config
{
  // General
  bool ShowFatalErrorMessagebox = true;

// Specific Linux
#if defined(__linux__)
  bool PreferWayland = true;
#endif

  // Raccoon Renderer
  bool Vsync      = true;
  bool BitDepth10 = false;

  // Change
  void * TriggeredChange = nullptr;
};
}
