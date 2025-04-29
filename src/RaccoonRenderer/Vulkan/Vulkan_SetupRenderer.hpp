#include <future>

#include <fast_io.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#if PATACHE_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif
#include <cglm/cglm.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
#include "fast_io_buff.hpp"

PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT
    = nullptr;

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT (VkInstance                    instance,
                                 VkDebugUtilsMessengerEXT      messenger,
                                 VkAllocationCallbacks const * pAllocator)
{
  return pfnVkDestroyDebugUtilsMessengerEXT (instance, messenger, pAllocator);
}
