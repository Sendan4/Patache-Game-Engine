#pragma once

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <yaml-cpp/yaml.h>
#include <fast_io.h>

#include "PatataEngine/PatataEngine.hpp"
#include "RaccoonRenderer.hpp"
#include "Config.hpp"

namespace Patata
{
class Engine::EngineImpl
{
public:
  EngineImpl (const std::string &, const uint32_t &, const uint32_t &);
  ~EngineImpl (void);

  Patata::Config Configuration;

  void HandleEvent (SDL_Event &);
  void BeginRender (void);
  void EndRender (void);
  void ClearColor (const float & R, const float & G, const float & B, const float & A);

private:
  SDL_Window *                        GameWindow      = nullptr;
  Patata::Graphics::RaccoonRenderer * RaccoonRenderer = nullptr;
  bool WindowResized = false;

  bool LoadConfig (Config &);

  void CreateGameWindow (const std::string &, const uint32_t &,
                         const uint32_t &);

  #if defined(USE_ICON)
  void SetWindowIcon (void);
  #endif

#if defined(DEBUG)
  void SetupImGUIBackend (void);
  void ImGuiStartFrame (void);
  void DrawDebugUI (void);
  void ImGuiEndFrame (void);
#endif
};

class RunTimeError : public std::exception
{
private:
  std::string message;

public:
  RunTimeError (const std::string & msg) : message (msg) {}
  std::string
  what (void)
  {
    return message;
  }
};
} // namespace Patata
