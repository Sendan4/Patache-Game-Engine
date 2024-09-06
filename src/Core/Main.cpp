#if defined(__linux__)
#include <cstdlib>
#endif
#include <future>

#include <fast_io.h>
#if defined(_WIN64)
#include <windows.h>
#endif

#include <vulkan/vulkan.hpp>

// Patata Engine
#include "Log.hpp"

#include "Config.hpp"
#include "PatataEngine/PatataEngine.hpp"
#include "PatataEngineImpl.hpp"

// Private API
Patata::Engine::EngineImpl::EngineImpl (const std::string & WindowTitle)
{
  {
     std::future<void> PatataStarLogInfo = std::async(
         std::launch::async, Patata::Log::StartPatataLogInfo);
  }

  // TODO : async load config
  if (!LoadConfig(Configuration)) return;

#if defined(__linux__)
  if (Configuration.PreferWayland)
    if (setenv ("SDL_VIDEODRIVER", "wayland", 1) != 0)
        std::future<void> Err = std::async(std::launch::async,
           Patata::Log::ErrorMessage,
           "Cannot set enviroment varible SDL_VIDEODRIVER");

  if (Configuration.EnableMangoHud) {
      if (setenv ("MANGOHUD", "1", 1) != 0)
        std::future<void> Err = std::async(std::launch::async,
            Patata::Log::ErrorMessage,
            "Cannot set enviroment varible MANGOHUD");
  }
#endif

#if defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
#if defined(_WIN64)
  if (SetEnvironmentVariable ("VK_LAYER_PATH", SDL_GetBasePath ()) == 0)
    std::future<void> Err = std::async(std::launch::async,
      Patata::Log::ErrorMessage,
      "Cannot set enviroment varible VK_LAYER_PATH");
#else
  if (setenv ("VK_LAYER_PATH", SDL_GetBasePath (), 1) != 0)
    std::future<void> Err = std::async(std::launch::async,
        Patata::Log::ErrorMessage,
        "Cannot set enviroment varible VK_LAYER_PATH");
#endif
#endif

#if defined(PATATA_FIND_VVL_FROM_SDK) && defined(_WIN64)
  if (SetEnvironmentVariable ("VK_LAYER_PATH", PATATA_VVL_SDK_PATH) == 0)
    std::future<void> Err = std::async(std::launch::async,
        Patata::Log::ErrorMessage,
        "Cannot set enviroment varible VK_LAYER_PATH");
#endif

  // Wait LoadConfig here
  // SDL Subsystems
  if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
      std::future<void> Err = std::async(std::launch::async,
            Patata::Log::FatalErrorMessage,
            "Patata Engine - SDL2",
            "Cannot init the video subsystem",
            Configuration);

      return;
    }

  if (SDL_Init (SDL_INIT_EVENTS) != 0)
    {
        std::future<void> Err = std::async(std::launch::async,
              Patata::Log::FatalErrorMessage,
              "Patata Engine - SDL2",
              "Cannot init the events subsystem",
              Configuration);

        return;
    }

  if (SDL_Init (SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::future<void> Err = std::async(std::launch::async,
              Patata::Log::FatalErrorMessage,
              "Patata Engine - SDL2",
              "Cannot init the gamecontroller subsystem",
              Configuration);

        return;
    }

  CreateGameWindow (WindowTitle);
#if defined(USE_ICON)
  SetWindowIcon ();
#endif

  RaccoonRenderer = new Patata::Graphics::RaccoonRenderer (Configuration, GameWindow, WindowResized);

#if defined(DEBUG)
  SetupImGUIBackend ();
#endif
}

// ExitLog.hpp Dependencies
#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif
#if defined(_WIN64)
#include <Windows.h>
#endif

#include "TerminalColors.hpp"
#include "ExitLog.hpp"

Patata::Engine::EngineImpl::~EngineImpl (void)
{
  Patata::Log::DeleteAndLogPtr ("Deallocate Raccoon Renderer", RaccoonRenderer);

  SDL_DestroyWindow (GameWindow);
}
