#if defined(__linux__)
#include <cstdlib>
#endif
#include <future>

#if defined(DEBUG)
#include <imgui_impl_vulkan.h>
#include <SDL_syswm.h>
#endif
#include <SDL.h>
#if defined(_WIN64)
#include <windows.h>
#endif
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
// PatataEngineImpl.hpp
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "PatataEngineImpl.hpp"
#include "Log.hpp"

// Private API
Patata::Engine::EngineImpl::EngineImpl (const char * WindowTitle)
{
  {
     std::future<void> PatataStarLogInfo = std::async(
         std::launch::async, Patata::Log::StartPatataLogInfo);
  }

  if (!LoadConfig(Configuration)) return;

  /*
  Async change enviroment variables
  Linux specific variables
  */
#if defined(__linux__)
  std::future<int> setenv_PreferWayland_Async;

  if (Configuration.PreferWayland)
    setenv_PreferWayland_Async = std::async(std::launch::async,
        setenv, "SDL_VIDEODRIVER", "wayland", 1);

  std::future<int> setenv_EnableMangoHud_Async;

  if (Configuration.EnableMangoHud)
    setenv_EnableMangoHud_Async = std::async(std::launch::async,
        setenv, "MANGOHUD", "1", 1);
#endif

  // Vulkan VVL VK_LAYER_PATH
#if defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
    std::future<int> setenv_VK_LAYER_PATH_Async = std::async(std::launch::async,
        #if defined(_WIN64)
        SetEnvironmentVariable, "VK_LAYER_PATH", SDL_GetBasePath ()
        #else
        setenv, "VK_LAYER_PATH", SDL_GetBasePath (), 1
        #endif
    );
#endif

#if defined(PATATA_FIND_VVL_FROM_SDK) && defined(_WIN64)
    std::future<int> setenv_VK_LAYER_PATH_Async = std::async(std::launch::async,
        SetEnvironmentVariable, "VK_LAYER_PATH", PATATA_VVL_SDK_PATH);
#endif

    // Linux especific variables
#if defined(__linux__)
    if (Configuration.PreferWayland) {
        setenv_PreferWayland_Async.wait();
        if (setenv_PreferWayland_Async.get() != 0)
            std::future<void> Err = std::async(std::launch::async,
                Patata::Log::ErrorMessage,
                "Cannot set enviroment varible SDL_VIDEODRIVER");
    }

    if (Configuration.EnableMangoHud) {
        setenv_EnableMangoHud_Async.wait();
        if (setenv_EnableMangoHud_Async.get() != 0)
            std::future<void> Err = std::async(std::launch::async,
                Patata::Log::ErrorMessage,
                "Cannot set enviroment varible MANGOHUD");
    }
#endif

    // Vulkan VVL VK_LAYER_PATH
#if defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH) || defined(PATATA_FIND_VVL_FROM_SDK)
    setenv_VK_LAYER_PATH_Async.wait();
#if defined(_WIN64) && (defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH) || defined(PATATA_FIND_VVL_FROM_SDK))
    if (setenv_VK_LAYER_PATH_Async.get() == 0)
#elif !defined(_WIN64) && defined(PATATA_FIND_VVL_IN_THE_CURRENT_PATH)
    if (setenv_VK_LAYER_PATH_Async.get() != 0)
#endif
        std::future<void> Err = std::async(std::launch::async,
            Patata::Log::ErrorMessage,
            "Cannot set enviroment varible VK_LAYER_PATH");
#endif

// SDL Subsystems
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0)
    {
      std::future<void> Err = std::async(std::launch::async,
            Patata::Log::FatalErrorMessage,
            "Patata Engine - SDL2",
            SDL_GetError(),
            Configuration);

      return;
    }

  CreateGameWindow (WindowTitle);

#if defined(USE_ICON)
  SetWindowIcon ();
#endif

#if defined(DEBUG)
  InitImgui ();
#endif

  RaccoonRenderer = new Patata::RaccoonRenderer(&RaccoonInfo);
}

#if defined(DEBUG)
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#endif

Patata::Engine::EngineImpl::~EngineImpl (void)
{
  delete RaccoonRenderer;

  // Imgui
  #if defined(DEBUG)
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  #endif

  // SDL
  SDL_DestroyWindow (GameWindow);
  SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
  SDL_Quit();
}
