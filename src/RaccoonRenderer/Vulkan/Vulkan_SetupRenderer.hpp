#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif
#include <future>
#include <functional>

#include <fast_io.h>
#include <fast_io_device.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#if PATACHE_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif
// #include <cglm/cglm.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
#include "fast_io_buff.hpp"
#include "CstringWrapped.hpp"
#if PATACHE_DEBUG == 1
#include "PatacheEngine/StructEngineInfo.hpp"
#endif

#define PATACHE_ENGINE_VERSION_VK                                             \
  VK_MAKE_API_VERSION (0, PATACHE_ENGINE_VERSION_MAYOR,                       \
                       PATACHE_ENGINE_VERSION_MINOR,                          \
                       PATACHE_ENGINE_VERSION_PATCH)

#define PATACHE_ERROR_TEXT_SIZE 256

#if PATACHE_DEBUG == 1
// Function pointers
// vkCreateDebugUtilsMessengerEXT
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT = nullptr;

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT (
    VkInstance                                 instance,
    const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
    const VkAllocationCallbacks *              pAllocator,
    VkDebugUtilsMessengerEXT *                 pMessenger)
{
  return pfnVkCreateDebugUtilsMessengerEXT (instance, pCreateInfo, pAllocator,
                                            pMessenger);
}

// vkDestroyDebugUtilsMessengerEXT
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT
    = nullptr;

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT (VkInstance                    instance,
                                 VkDebugUtilsMessengerEXT      messenger,
                                 VkAllocationCallbacks const * pAllocator)
{
  return pfnVkDestroyDebugUtilsMessengerEXT (instance, messenger, pAllocator);
}

// Callback Message function
VKAPI_ATTR vk::Bool32 VKAPI_CALL
debugMessageFunc (vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  vk::DebugUtilsMessageTypeFlagsEXT        messageTypes,
                  vk::DebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                  void * /*pUserData*/)
{
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                        PATACHE_TERM_COLOR_RED,
                        "Vulkan Validation Layers :", PATACHE_TERM_RESET);

  // Message Severity
  switch (messageSeverity)
    {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
      fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT,
                            fast_io::mnp::right ("Severity : ", 13),
                            vk::to_string (messageSeverity));
      break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
      fast_io::io::println (
          PATACHE_FAST_IO_BUFF_OUT, fast_io::mnp::right ("Severity : ", 13),
          PATACHE_TERM_COLOR_YELLOW, vk::to_string (messageSeverity),
          PATACHE_TERM_RESET);
      break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
      fast_io::io::println (
          PATACHE_FAST_IO_BUFF_OUT, fast_io::mnp::right ("Severity : ", 13),
          PATACHE_TERM_COLOR_RED, vk::to_string (messageSeverity),
          PATACHE_TERM_RESET);
      break;
    }

  // Message Types
  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT,
                          fast_io::mnp::right ("Type : ", 9),
                          vk::to_string (messageTypes));

  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
      || messageTypes
             & vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding)
    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT,
                          fast_io::mnp::right ("Type : ", 9),
                          PATACHE_TERM_COLOR_YELLOW,
                          vk::to_string (messageTypes), PATACHE_TERM_RESET);

  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT,
                          fast_io::mnp::right ("Type : ", 9),
                          PATACHE_TERM_COLOR_BLUE,
                          vk::to_string (messageTypes), PATACHE_TERM_RESET);

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT, fast_io::mnp::right ("ID Name : ", 12),
      fast_io::mnp::os_c_str (pCallbackData->pMessageIdName), "\n",
      fast_io::mnp::right ("ID Number : ", 14), pCallbackData->messageIdNumber,
      "\n", fast_io::mnp::right ("Message : ", 12),
      fast_io::mnp::os_c_str (pCallbackData->pMessage), "\n");

  return false;
}
#endif

// Patache Engine
bool CreateSwapChain (Patache::Engine * const, Patache::SwapChainInfo &);
bool CreateImageView (Patache::VulkanBackend &,
                      const Patache::SwapChainInfo &);
bool CreateFrameBuffer (Patache::VulkanBackend &);
bool CreateDepthBuffer (Patache::VulkanBackend &, Patache::Config &);
bool CreateSemaphores (Patache::VulkanBackend &);
bool CreateCommandBuffer (Patache::VulkanBackend &);
bool CreateFence (Patache::VulkanBackend &);
void VulkanInfo (Patache::Engine * const, const Patache::SwapChainInfo &);

#if PATACHE_DEBUG == 1
void InitImgui (const Patache::Config &);
bool InitImguiVulkan (Patache::Engine * const);
bool CreateImguiDescriptorPool (Patache::VulkanBackend &);
bool CreateImguiPipelineCache (Patache::VulkanBackend &);
#endif
