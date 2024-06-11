#if defined(__linux__)
#include <cstdlib>
#endif

#include <fast_io.h>

#if defined(_WIN64)
#include <windows.h>

extern "C"
{
  __declspec (dllexport) unsigned long NvOptimusEnablement        = 0x00000001;
  __declspec (dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

// Patata Engine
#include "Log.hpp"

#if defined(PATATA_GAME_NAME)
#define GAME_CONFIG_FILE_NAME PATATA_GAME_NAME ".yaml"
#else
#define GAME_CONFIG_FILE_NAME "patata.yaml"
#endif

#include "PatataEngineImpl.hpp"

// Public API
Patata::Engine::Engine (const std::string & WindowTitle,
                        const uint64_t & Width, const uint64_t & Height)
    : pPatataEngine (new EngineImpl (WindowTitle, Width, Height))
{
}

Patata::Engine::~Engine ()
{
  delete pPatataEngine;
  pPatataEngine = nullptr;
}

void
Patata::Engine::HandleEvent (SDL_Event & Event)
{
  pPatataEngine->HandleEvent (Event);
}

void
Patata::Engine::Render (void)
{
  pPatataEngine->Render ();
}

// Private API
Patata::Engine::EngineImpl::EngineImpl (const std::string & WindowTitle,
                                        const uint32_t &    WindowWidth,
                                        const uint32_t &    WindowHeight)
{
  Patata::Log::StartPatataLogInfo ();

  try
    {
      Config = YAML::LoadFile (
          strcpy (SDL_GetBasePath (), GAME_CONFIG_FILE_NAME));
    }
  catch (const YAML::BadFile & BadFile)
    {
      Patata::Log::YamlFileErrorMessage ();
      exit (1);
    }

#if defined(__linux__)
  if (Config["patata-engine"]["prefer-wayland"].as<bool> ())
    if (setenv ("SDL_VIDEODRIVER", "wayland", 1) != 0)
      Patata::Log::ErrorMessage (
          "Cannot set enviroment varible SDL_VIDEODRIVER");
#endif

#if defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
#if defined(_WIN64)
  if (SetEnvironmentVariable ("VK_LAYER_PATH", SDL_GetBasePath ()) == 0)
    Patata::Log::ErrorMessage ("Cannot set enviroment varible VK_LAYER_PATH");
#else
  if (setenv ("VK_LAYER_PATH", SDL_GetBasePath (), 1) != 0)
    Patata::Log::ErrorMessage ("Cannot set enviroment varible VK_LAYER_PATH");
#endif
#endif

#if defined(PATATA_FIND_VVL_FROM_SDK) && defined(_WIN64)
  if (SetEnvironmentVariable ("VK_LAYER_PATH", PATATA_VVL_SDK_PATH) == 0)
    Patata::Log::ErrorMessage ("Cannot set enviroment varible VK_LAYER_PATH");
#endif

  // SDL Subsystems
  if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
      Patata::Log::FatalErrorMessage (
          "SDL2", "Cannot init the video subsystem", Config);
      throw Patata::RunTimeError ("SDL Cannot init the video subsystem");
    }

  if (SDL_Init (SDL_INIT_EVENTS) != 0)
    {
      Patata::Log::FatalErrorMessage (
          "SDL2", "Cannot init the events subsystem", Config);
      throw Patata::RunTimeError ("SDL Cannot init the events subsystem");
    }

  if (SDL_Init (SDL_INIT_GAMECONTROLLER) != 0)
    {
      Patata::Log::FatalErrorMessage (
          "SDL2", "Cannot init the gamecontroller subsystem", Config);
      throw Patata::RunTimeError (
          "SDL Cannot init the GameController subsystem");
    }

  CreateGameWindow (WindowTitle, WindowWidth, WindowHeight);
#if defined(USE_ICON)
  SetWindowIcon ();
#endif

  RaccoonRenderer
      = new Patata::Graphics::RaccoonRenderer (Config, GameWindow);

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
  Patata::Log::DeleteAndLogPtr ("Raccoon Renderer", RaccoonRenderer);

  SDL_DestroyWindow (GameWindow);
}
