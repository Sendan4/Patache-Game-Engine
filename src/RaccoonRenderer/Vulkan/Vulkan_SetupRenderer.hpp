#include <cstring>
#include <future>
#include <functional>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <fast_io.h>
#include <fast_io_device.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"
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
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT{ nullptr };

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT (VkInstance                                 instance,
                                const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
                                const VkAllocationCallbacks *              pAllocator,
                                VkDebugUtilsMessengerEXT *                 pMessenger)
{
  return pfnVkCreateDebugUtilsMessengerEXT (instance, pCreateInfo, pAllocator, pMessenger);
}

// vkDestroyDebugUtilsMessengerEXT
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT{ nullptr };

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT (VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                 VkAllocationCallbacks const * pAllocator)
{
  return pfnVkDestroyDebugUtilsMessengerEXT (instance, messenger, pAllocator);
}

// Callback Message function
VKAPI_ATTR VkBool32 VKAPI_CALL
DebugMessageFunc (VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT              messageTypes,
                  VkDebugUtilsMessengerCallbackDataEXT const * pCallbackData, void * /*pUserData*/)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_RED,
                        "Vulkan Validation Layers :", PATACHE_TERM_RESET);

  // Message Severity
  switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
      fast_io::io::println (
          PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Severity : ", 13),
          fast_io::mnp::os_c_str (string_VkDebugUtilsMessageSeverityFlagBitsEXT (messageSeverity)));
      break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      fast_io::io::println (
          PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Severity : ", 13),
          PATACHE_TERM_COLOR_YELLOW,
          fast_io::mnp::os_c_str (string_VkDebugUtilsMessageSeverityFlagBitsEXT (messageSeverity)),
          PATACHE_TERM_RESET);
      break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      fast_io::io::println (
          PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Severity : ", 13), PATACHE_TERM_COLOR_RED,
          fast_io::mnp::os_c_str (string_VkDebugUtilsMessageSeverityFlagBitsEXT (messageSeverity)),
          PATACHE_TERM_RESET);
      break;
    }

  // Message Types
  switch (messageTypes)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Type : ", 9),
                            "VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT");
      break;

    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Type : ", 9),
                            PATACHE_TERM_COLOR_YELLOW,
                            "VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT", PATACHE_TERM_RESET);
      break;

    case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
      fast_io::io::println (
          PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Type : ", 9), PATACHE_TERM_COLOR_YELLOW,
          "VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT", PATACHE_TERM_RESET);
      break;

    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::right ("Type : ", 9),
                            PATACHE_TERM_COLOR_BLUE,
                            "VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT", PATACHE_TERM_RESET);
      break;
    }

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
