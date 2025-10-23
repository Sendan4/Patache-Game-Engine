#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif
#include <future>
#include <functional>

#include <vulkan/vulkan.hpp>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <fast_io.h>
#include <fast_io_device.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"
#include "LogMacroUtils.hpp"
#include "CstringWrapped.hpp"
#include "Vulkan_SetupLog.hpp"
#include "Message.hpp"
#include "Vulkan_SetupRenderer_Funcs.hpp"

#define PATACHE_ENGINE_VERSION_VK                                                                  \
  VK_MAKE_API_VERSION (0, PATACHE_ENGINE_VERSION_MAYOR, PATACHE_ENGINE_VERSION_MINOR,              \
                       PATACHE_ENGINE_VERSION_PATCH)

#define PATACHE_ERROR_TEXT_SIZE 256

#if PATACHE_DEBUG == 1
// Function pointers

// vkCreateDebugUtilsMessengerEXT
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT = nullptr;

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT (VkInstance                                 instance,
                                const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
                                const VkAllocationCallbacks *              pAllocator,
                                VkDebugUtilsMessengerEXT *                 pMessenger)
{
  return pfnVkCreateDebugUtilsMessengerEXT (instance, pCreateInfo, pAllocator, pMessenger);
}

// vkDestroyDebugUtilsMessengerEXT
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT = nullptr;

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT (VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                 VkAllocationCallbacks const * pAllocator)
{
  return pfnVkDestroyDebugUtilsMessengerEXT (instance, messenger, pAllocator);
}

// Callback Message function
VKAPI_ATTR vk::Bool32 VKAPI_CALL
           DebugMessageFunc (vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                             vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
                             vk::DebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                             void * /*pUserData*/)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_RED,
                        "Vulkan Validation Layers :", PATACHE_TERM_RESET);

  // Message Severity
  switch (messageSeverity)
    {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Severity : ", 13),
                            vk::to_string (messageSeverity));
      break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Severity : ", 13),
                            PATACHE_TERM_COLOR_YELLOW, vk::to_string (messageSeverity),
                            PATACHE_TERM_RESET);
      break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Severity : ", 13),
                            PATACHE_TERM_COLOR_RED, vk::to_string (messageSeverity),
                            PATACHE_TERM_RESET);
      break;
    }

  // Message Types
  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Type : ", 9),
                          vk::to_string (messageTypes));

  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
      || messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding)
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Type : ", 9),
                          PATACHE_TERM_COLOR_YELLOW, vk::to_string (messageTypes),
                          PATACHE_TERM_RESET);

  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Type : ", 9),
                          PATACHE_TERM_COLOR_BLUE, vk::to_string (messageTypes),
                          PATACHE_TERM_RESET);

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("ID Name : ", 12),
                        fast_io::mnp::os_c_str (pCallbackData->pMessageIdName), "\n",
                        fast_io::mnp::right ("ID Number : ", 14), pCallbackData->messageIdNumber,
                        "\n", fast_io::mnp::right ("Message : ", 12),
                        fast_io::mnp::os_c_str (pCallbackData->pMessage), "\n");

  return false;
}
#endif

// Patache Engine
// Vulkan_Swapchain.cpp Func
bool CreateSwapchain (Patache::Engine * const, Patache::SwapchainInfo &);
// Vulkan_ImageView.cpp Func
bool CreateImageView (Patache::VulkanBackend &, const Patache::SwapchainInfo &);
// Vulkan_FrameBuffer.cpp Func
bool CreateFrameBuffer (Patache::VulkanBackend &);
// Vulkan_DepthBuffer.cpp Func
bool CreateDepthBuffer (Patache::VulkanBackend &, Patache::Config &);
// Vulkan_SincronizationPrimitives.cpp Funcs
bool CreateSemaphores (Patache::VulkanBackend &);
bool CreateCommandBuffer (Patache::VulkanBackend &);
bool CreateFence (Patache::VulkanBackend &);
// Vulkan_Info.cpp Func
void VulkanInfo (Patache::Engine * const, const Patache::SwapchainInfo &);

#if PATACHE_DEBUG == 1
// SetupImgui.cpp Funcs
void InitImGuiCore (const Patache::Config &, Patache::EngineInfo &);
bool InitImGuiVulkan (Patache::Engine * const);
bool CreateImguiDescriptorPool (Patache::VulkanBackend &);
bool CreateImguiPipelineCache (Patache::VulkanBackend &);
#endif
