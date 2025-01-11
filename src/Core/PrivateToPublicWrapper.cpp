// All internal functions that will be used publicly must be wrapped
// Engine Implementation (Wrapping) -> Public API (Wrapping)

#if PATATA_DEBUG == 1
#include <SDL_syswm.h>
#include <imgui_impl_vulkan.h>
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#if PATATA_DEBUG == 1
#include "StructEngineInfo.hpp"
#endif
// Begin Dependencies of PatataEngineImpl.hpp
#include "StructConfig.hpp"
#include "Log.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
// End Dependencies of PatataEngineImpl.hpp
#include "PatataEngineImpl.hpp"

// Public API
Patata::Engine::Engine (const char * WindowTitle)
    : pPatataEngine (new EngineImpl (WindowTitle))
{
  pClearColor = &pPatataEngine->ClearColor;
}

Patata::Engine::Engine (void) : pPatataEngine (new EngineImpl (nullptr))
{
  pClearColor = &pPatataEngine->ClearColor;
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

bool
Patata::Engine::BeginRender (SDL_Event & Event)
{
  return pPatataEngine->BeginRender (Event);
}

void
Patata::Engine::EndRender (SDL_Event & Event)
{
  pPatataEngine->EndRender (Event);
}
