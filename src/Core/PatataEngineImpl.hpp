#pragma once

namespace Patata
{
class Engine::EngineImpl
{
public:
  EngineImpl (const char *);
  ~EngineImpl (void);

  Patata::Config     Configuration;
  Patata::ClearColor ClearColor{};

  void HandleEvent (SDL_Event &);
  bool BeginRender (SDL_Event &);
  void EndRender (SDL_Event &);

private:
#if PATATA_DEBUG == 1
  Patata::EngineInfo PatataEngineInfo;
#endif
  SDL_Window *                      GameWindow = nullptr;
  Patata::RaccoonRendererCreateInfo RaccoonInfo{
    .pConfiguration = &Configuration,
    .ppWindow       = &GameWindow,
    .pWindowResized = &WindowResized,
    .pClearColor    = &ClearColor,
#if PATATA_DEBUG == 1
    .pPatataEngineInfo = &PatataEngineInfo
#endif
  };
  Patata::RaccoonRenderer * RaccoonRenderer = nullptr;
  bool                      WindowResized   = false;

  bool LoadConfig (Config &);

  void CreateGameWindow (const char *);

#if defined(USE_ICON)
  void SetWindowIcon (void);
#endif

#if PATATA_DEBUG == 1
  void InitImgui (void);
#endif
};
} // namespace Patata
