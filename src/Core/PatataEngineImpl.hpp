#pragma once

#include <csignal>
#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <yaml-cpp/yaml.h>
#include <fast_io.h>

#include "PatataEngine/PatataEngine.hpp"
#include "RaccoonRenderer.hpp"
#include "Config.hpp"
#if defined(DEBUG)
#include "EngineInfo.hpp"
#endif

namespace Patata
{
class Engine::EngineImpl
{
public:
  EngineImpl (const std::string &);
  ~EngineImpl (void);

  Patata::Config Configuration;
  Patata::ClearColor ClearColor {};

  void HandleEvent (SDL_Event &);
  bool BeginRender (SDL_Event &);
  void EndRender (SDL_Event &);

private:
  SDL_Window *                        GameWindow      = nullptr;
  Patata::Graphics::RaccoonRenderer * RaccoonRenderer = nullptr;
  bool WindowResized = false;
  #if defined(DEBUG)
  Patata::EngineInfo PatataEngineInfo;
  #endif

  Patata::Graphics::RaccoonRendererCreateInfo RaccoonInfo {
    .pConfiguration = &Configuration,
    .ppWindow = &GameWindow,
    .pWindowResized = &WindowResized,
    .pClearColor = &ClearColor,
    #if defined(DEBUG)
    .pPatataEngineInfo = &PatataEngineInfo
    #endif
  };

  bool LoadConfig (Config &);

  void CreateGameWindow (const std::string &);

  #if defined(USE_ICON)
  void SetWindowIcon (void);
  #endif

#if defined(DEBUG)
  void InitialImguiSetup (void);
#endif
};
} // namespace Patata
