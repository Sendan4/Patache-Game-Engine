#include <future>

#include <fast_io.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#if PATATA_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"

PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT
    = nullptr;

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT (VkInstance                    instance,
                                 VkDebugUtilsMessengerEXT      messenger,
                                 VkAllocationCallbacks const * pAllocator)
{
  return pfnVkDestroyDebugUtilsMessengerEXT (instance, messenger, pAllocator);
}
