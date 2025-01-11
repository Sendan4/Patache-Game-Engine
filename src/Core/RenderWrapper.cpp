// Raccoon Renderer Functions (Wrapping) -> Engine Implementation (Wrapping)

#if PATATA_DEBUG == 1
#include <SDL_syswm.h>
#include <imgui_impl_vulkan.h>
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
// Begin Dependencies of PatataEngineImpl.hpp
#if PATATA_DEBUG == 1
#include "StructEngineInfo.hpp"
#endif
#include "StructConfig.hpp"
#include "Log.hpp"
#include "PatataEngine/StructClearColor.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
// End Dependencies of PatataEngineImpl.hpp
#include "PatataEngineImpl.hpp"

bool
Patata::Engine::EngineImpl::BeginRender (SDL_Event & Event)
{
  return RaccoonRenderer->BeginRender (Event);
}

void
Patata::Engine::EngineImpl::EndRender (SDL_Event & Event)
{
  RaccoonRenderer->EndRender (Event);
}
