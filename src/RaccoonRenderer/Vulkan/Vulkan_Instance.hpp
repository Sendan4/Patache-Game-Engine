#include <future>
#include <cstring>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "Log.hpp"
#include "CstringWrapped.hpp"
#include "fast_io_buff.hpp"

#define PATATA_ENGINE_VERSION_VK                                              \
  VK_MAKE_API_VERSION (0, PATATA_ENGINE_VERSION_MAYOR,                        \
                       PATATA_ENGINE_VERSION_MINOR,                           \
                       PATATA_ENGINE_VERSION_PATCH)

#define PATATA_ERROR_TEXT_SIZE 256

#if PATATA_DEBUG == 1
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

#include <fast_io.h>
#include "ColorTerminal.hpp"

// Callback Message function
VKAPI_ATTR vk::Bool32 VKAPI_CALL
debugMessageFunc (vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  vk::DebugUtilsMessageTypeFlagsEXT        messageTypes,
                  vk::DebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                  void * /*pUserData*/)
{
  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD, PATATA_TERM_COLOR_RED,
      "Vulkan Validation Layers :", PATATA_TERM_RESET, "\n Message Info : ");

  // Message Severity
  switch (messageSeverity)
    {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
      fast_io::io::println (PATATA_FAST_IO_BUFF_OUT,
                            fast_io::mnp::right ("Severity : ", 13),
                            vk::to_string (messageSeverity));
      break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
      fast_io::io::println (
          PATATA_FAST_IO_BUFF_OUT, fast_io::mnp::right ("Severity : ", 13),
          PATATA_TERM_COLOR_YELLOW, vk::to_string (messageSeverity),
          PATATA_TERM_RESET);
      break;

    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
      fast_io::io::println (
          PATATA_FAST_IO_BUFF_OUT, fast_io::mnp::right ("Severity : ", 13),
          PATATA_TERM_COLOR_RED, vk::to_string (messageSeverity),
          PATATA_TERM_RESET);
      break;
    }

  // Message Types
  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
    {
      fast_io::io::println (PATATA_FAST_IO_BUFF_OUT,
                            fast_io::mnp::right ("Type : ", 9),
                            vk::to_string (messageTypes));
    }

  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
      || messageTypes
             & vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding)
    {
      fast_io::io::println (PATATA_FAST_IO_BUFF_OUT,
                            fast_io::mnp::right ("Type : ", 9),
                            PATATA_TERM_COLOR_YELLOW,
                            vk::to_string (messageTypes), PATATA_TERM_RESET);
    }

  if (messageTypes & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
    {
      fast_io::io::println (PATATA_FAST_IO_BUFF_OUT,
                            fast_io::mnp::right ("Type : ", 9),
                            PATATA_TERM_COLOR_BLUE,
                            vk::to_string (messageTypes), PATATA_TERM_RESET);
    }

  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT, fast_io::mnp::right ("ID Name : ", 11),
      fast_io::mnp::os_c_str (pCallbackData->pMessageIdName), "\n",
      fast_io::mnp::right ("ID Number : ", 14), pCallbackData->messageIdNumber,
      "\n", fast_io::mnp::right ("Message : ", 11),
      fast_io::mnp::os_c_str (pCallbackData->pMessage), "\n");

  return false;
}
#endif
