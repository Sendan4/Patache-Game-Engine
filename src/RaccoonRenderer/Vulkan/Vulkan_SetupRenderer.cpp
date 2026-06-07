#include <cstring>
#include <future>
#include <cstdint>

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

#if PATACHE_LINUX_OR_UNIX
extern std::uint8_t scaleInt;
#endif

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
#include "Vulkan_Swapchain.hpp"
#include "Vulkan_ImageView.hpp"
#include "Vulkan_FrameBuffer.hpp"
// #include "Vulkan_DepthBuffer.hpp"
#include "Vulkan_CommandBuffer.hpp"
#include "Vulkan_SincronizationPrimitives.hpp"
#include "Vulkan_Info.hpp"

#if PATACHE_DEBUG == 1
  #include "SetupImgui.hpp"
#endif

#include "Vulkan_SetupRenderer.hpp"

bool
Patache::RaccoonRendererInit (Patache::Engine * pEngine, const Patache::EngineCreateInfo & rInfo)
{
#if PATACHE_DEBUG == 1
  std::future<void> initImguiCore_Async{ std::async (std::launch::async, Patache::InitImGuiCore,
                                                     std::cref (pEngine->configuration),
                                                     std::ref (pEngine->debugInfo)) };
#endif

  VkResult result;

#if PATACHE_DEBUG == 1
  bool vkEXTDebugUtilsExtensionIsEnabled{ false };
#endif

  // Verify vulkan version
  {
    PFN_vkVoidFunction PFN_vkEnumerateInstanceVersion{ vkGetInstanceProcAddr (
        nullptr, "vkEnumerateInstanceVersion") };

    if (PFN_vkEnumerateInstanceVersion == nullptr)
      {
        Patache::FatalErrorMessage (
            "Patache Engine - Raccoon Renderer",
            "Your drivers are too old, your vulkan version is unsupported. (Vulkan 1.1 <=)",
            pEngine->configuration);

        return false;
      }

    std::uint32_t supportedVersion{ 0U };
    result = vkEnumerateInstanceVersion (&supportedVersion);

    if (result != VK_SUCCESS)
      {
        Patache::WarningMessage ("Cannot get the supported vulkan version");

        return false;
      }

    if (supportedVersion < 4202496U)
      {
        Patache::FatalErrorMessage (
            "Patache Engine - Raccoon Renderer",
            "Your drivers are too old, your vulkan version is unsupported. (Vulkan 1.1 <=)",
            pEngine->configuration);

        return false;
      }
  }

  // Vulkan Instance
  {
    const char * const pGameName{ (rInfo.pGameName != nullptr) ? rInfo.pGameName
                                                               : "Untitled Game" };

    const VkApplicationInfo patacheEngineInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                               .pNext = nullptr,
                                               .pApplicationName   = pGameName,
                                               .applicationVersion = rInfo.gameVersion,
                                               .pEngineName        = PATACHE_ENGINE_NAME,
                                               .engineVersion      = PATACHE_ENGINE_VERSION_VK,
                                               .apiVersion         = VK_API_VERSION_1_2 };

    // The validation layers are activated with USE_VVL=ON.
    // They are for the development and testing of this backend.
    // Layers
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
    pEngine->debugInfo.ppLayersVK
        = static_cast<const char **> (std::calloc (PATACHE_LAYER_VK_COUNT, sizeof (const char *)));

    pEngine->debugInfo.ppLayersVK[0] = "VK_LAYER_KHRONOS_validation";

    Patache::VulkanList (pEngine->debugInfo.ppLayersVK, PATACHE_LAYER_VK_COUNT, "Layers");
#endif

    // Get Extensions
    std::uint32_t instanceExtensionCount{ 0U }, extensionCountSDL{ 0U };

    const char * const * pArrayInstanceExtensionsSDL
        = SDL_Vulkan_GetInstanceExtensions (&extensionCountSDL);

    instanceExtensionCount += extensionCountSDL;

#if PATACHE_DEBUG == 1
  #if defined(PATACHE_USE_VVL)
    ++instanceExtensionCount; // VK_EXT_layer_settings is no reported o enumerated
  #endif
#endif

    // Search extension instance
    VkExtensionProperties * pInstanceExtensionProperties{ nullptr };
    std::uint32_t           propertyCount{ 0U };

    result = vkEnumerateInstanceExtensionProperties (nullptr, &propertyCount, nullptr);

    if (result != VK_SUCCESS)
      {
        Patache::WarningMessage (
            "vkEnumerateInstanceVersion() Cannot get the instance extensions count. continuing "
            "without some extensions");

        goto SKIP_OTHER_EXTENSIONS;
      }

    pInstanceExtensionProperties = static_cast<VkExtensionProperties *> (
        std::calloc (propertyCount, sizeof (VkExtensionProperties)));

    if (pInstanceExtensionProperties == nullptr)
      {
        return false;
      }

    result = vkEnumerateInstanceExtensionProperties (nullptr, &propertyCount,
                                                     pInstanceExtensionProperties);

    if (result != VK_SUCCESS)
      {
        Patache::WarningMessage (
            "vkEnumerateInstanceVersion() Cannot get the instance extensions. continuing without "
            "some extensions");

        if (pInstanceExtensionProperties != nullptr)
          {
            std::free (pInstanceExtensionProperties);
          }

        goto SKIP_OTHER_EXTENSIONS;
      }

#if PATACHE_DEBUG == 1
  #if defined(PATACHE_USE_VVL)
    for (std::uint32_t i{ 0U }; i < propertyCount; ++i)
      {
        if (std::strcmp (pInstanceExtensionProperties[i].extensionName,
                         VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
            == 0U)
          {
            ++instanceExtensionCount;
            vkEXTDebugUtilsExtensionIsEnabled = true;
            break;
          }
      }
  #endif
#endif

    if (pInstanceExtensionProperties != nullptr)
      {
        std::free (pInstanceExtensionProperties);
        pInstanceExtensionProperties = nullptr;
      }

  SKIP_OTHER_EXTENSIONS:

    // Enable instance extensions
    const char ** ppAllExtensionInstance{ nullptr };

    if (pArrayInstanceExtensionsSDL != nullptr)
      {
        // ppAllExtensionInstance = new const char *[instanceExtensionCount](nullptr);
        ppAllExtensionInstance = static_cast<const char **> (
            std::calloc (instanceExtensionCount, sizeof (const char *)));

        PATACHE_MEMCPY (ppAllExtensionInstance, pArrayInstanceExtensionsSDL,
                        extensionCountSDL * sizeof (const char *),
                        extensionCountSDL * sizeof (const char *));

#if PATACHE_DEBUG == 1
        // extensionCountSDL is the last element in the index of SDL extensions. here is used as a
        // index for other extensions.
        if (ppAllExtensionInstance[extensionCountSDL] != nullptr)
          {
            ++extensionCountSDL;
          }

        if (vkEXTDebugUtilsExtensionIsEnabled)
          {
            ppAllExtensionInstance[extensionCountSDL] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
          }

  #if defined(PATACHE_USE_VVL)
        ppAllExtensionInstance[(++extensionCountSDL)] = VK_EXT_LAYER_SETTINGS_EXTENSION_NAME;
  #endif
#endif

#if PATACHE_DEBUG == 1
        // Copy to debug info
        pEngine->debugInfo.instanceExtensionsCountVK = instanceExtensionCount;
        pEngine->debugInfo.ppInstanceExtensionsVK    = static_cast<const char **> (
            std::calloc (instanceExtensionCount, sizeof (const char *)));

        PATACHE_MEMCPY (pEngine->debugInfo.ppInstanceExtensionsVK, ppAllExtensionInstance,
                        instanceExtensionCount * sizeof (const char *),
                        instanceExtensionCount * sizeof (const char *));
#endif

        Patache::VulkanList (ppAllExtensionInstance, instanceExtensionCount, "Instance Extensions");
      }
    else
      {
        Patache::FatalErrorMessage ("Patache Engine - Raccoon Renderer",
                                    "Cannot find vulkan surface extensions",
                                    pEngine->configuration);

        return false;
      }

      // Configuration of Validation Layers
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
    static constexpr VkBool32 enableSetting{ VK_TRUE };

    constexpr VkLayerSettingEXT settings[]{ {
                                                .pLayerName   = "VK_LAYER_KHRONOS_validation",
                                                .pSettingName = "thread_safety",
                                                .type         = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
                                                .valueCount   = 1U,
                                                .pValues      = &enableSetting,
                                            },
                                            {
                                                .pLayerName   = "VK_LAYER_KHRONOS_validation",
                                                .pSettingName = "validate_sync",
                                                .type         = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
                                                .valueCount   = 1U,
                                                .pValues      = &enableSetting,
                                            },
                                            {
                                                .pLayerName   = "VK_LAYER_KHRONOS_validation",
                                                .pSettingName = "validate_best_practices",
                                                .type         = VK_LAYER_SETTING_TYPE_BOOL32_EXT,
                                                .valueCount   = 1U,
                                                .pValues      = &enableSetting,
                                            } };

    const VkLayerSettingsCreateInfoEXT layerSettingsInfo{
      .sType        = VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT,
      .pNext        = nullptr,
      .settingCount = 3U,
      .pSettings    = settings
    };

  #define EXTENT_STRUCT &layerSettingsInfo
#endif

#if !defined(EXTENT_STRUCT)
  #define EXTENT_STRUCT nullptr
#endif

    // Create Instance
    const VkInstanceCreateInfo instanceInfo{
      .sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext            = EXTENT_STRUCT,
      .flags            = 0U,
      .pApplicationInfo = &patacheEngineInfo,
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
      // Using Layers
      .enabledLayerCount   = PATACHE_LAYER_VK_COUNT,
      .ppEnabledLayerNames = pEngine->debugInfo.ppLayersVK,
#else
      // Not Using Layers
      .enabledLayerCount   = 0U,
      .ppEnabledLayerNames = nullptr,
#endif
      .enabledExtensionCount   = instanceExtensionCount,
      .ppEnabledExtensionNames = ppAllExtensionInstance,
    };

    result = vkCreateInstance (&instanceInfo, nullptr, &pEngine->vulkan.instance);

#if defined(EXTENT_STRUCT)
  #undef EXTENT_STRUCT
#endif

    std::free (ppAllExtensionInstance);
    ppAllExtensionInstance = nullptr;

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
      {
        char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

        PATACHE_STRNCPY (errorText,
                         "vkCreateInstance() You do not have Vulkan API compatible drivers or your "
                         "GPU does not support the Vulkan API. ",
                         PATACHE_ERROR_TEXT_SIZE - 1, PATACHE_ERROR_TEXT_SIZE);

        PATACHE_STRNCAT (errorText, string_VkResult (result), PATACHE_ERROR_TEXT_SIZE - 1,
                         PATACHE_ERROR_TEXT_SIZE);

        Patache::FatalErrorMessage ("Patache Engine - Raccoon Renderer", errorText,
                                    pEngine->configuration);

        return false;
      }

    if (result != VK_SUCCESS)
      {
        Patache::VulkanCheck ("vkCreateInstance()", result);

        return false;
      }
  }

#if PATACHE_DEBUG == 1
  // Debug Utils Messenger Info
  if (vkEXTDebugUtilsExtensionIsEnabled)
    {
      // Get function address
      pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT> (
          vkGetInstanceProcAddr (pEngine->vulkan.instance, "vkCreateDebugUtilsMessengerEXT"));

      if (pfnVkCreateDebugUtilsMessengerEXT == nullptr)
        {
          Patache::ErrorMessage ("vkGetInstanceProcAddr() Cannot get address of function "
                                 "vkCreateDebugUtilsMessengerEXT()");

          goto EXIT_CREATE_DEBUG_UTILS_MESSENGER;
        }

      // Debug Messenger Info
      const VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{
        .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext           = nullptr,
        .flags           = 0U,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &DebugMessageFunc,
        .pUserData       = nullptr
      };

      result = vkCreateDebugUtilsMessengerEXT (pEngine->vulkan.instance, &debugMessengerInfo,
                                               nullptr, &pEngine->vulkan.debugMessenger);

      if (result != VK_SUCCESS)
        {
          Patache::VulkanCheck ("vkCreateDebugUtilsMessengerEXT()", result);

          return false;
        }
    }
EXIT_CREATE_DEBUG_UTILS_MESSENGER:
#endif

  /*
  The GPU with the highest score wins to be chosen later as the logic device
  and also the physical device is used to obtain information about its
  characteristics.

  Discret video cards are preferred.
  Geometry shading support is obligatory.
  The larger the maximum image size the GPU can handle, the better.
  */

  VkPhysicalDeviceProperties2 physicalDeviceProperties{
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = nullptr, .properties{}
  };

  // Search a Physical Device
  {
    std::uint32_t gpuCount{ 0U };

    result = vkEnumeratePhysicalDevices (pEngine->vulkan.instance, &gpuCount, nullptr);

    if (result != VK_SUCCESS)
      {
        Patache::VulkanCheck ("vkEnumeratePhysicalDevices() Obtaining the count", result);

        return false;
      }

    if (gpuCount <= 0)
      {
        Patache::FatalErrorMessage ("Patache engine - Raccoon Renderer",
                                    "No vulkan compatible device found", pEngine->configuration);

        return false;
      }

    VkPhysicalDevice * pTmpPhysicalDevices{ static_cast<VkPhysicalDevice *> (
        std::malloc (sizeof (VkPhysicalDevice) * gpuCount)) };

    if (pTmpPhysicalDevices == nullptr)
      {
        return false;
      }

    result = vkEnumeratePhysicalDevices (pEngine->vulkan.instance, &gpuCount, pTmpPhysicalDevices);

    if (result != VK_SUCCESS)
      {
        Patache::VulkanCheck ("vkEnumeratePhysicalDevices()", result);

        Patache::FatalErrorMessage ("Patache Engine - RaccoonRenderer",
                                    "Failed to obtain a physical device", pEngine->configuration);

        if (pTmpPhysicalDevices != nullptr)
          {
            std::free (pTmpPhysicalDevices);
            pTmpPhysicalDevices = nullptr;
          }

        return false;
      }

    if (gpuCount == 1)
      {
        // If you only have one compatible GPU, I will take it directly.
        fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                              "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                              " : Only one vulkan compatible device found", PATACHE_TERM_RESET);

        // Select the only device available
        pEngine->vulkan.physicalDevice = pTmpPhysicalDevices[0];

        if (pTmpPhysicalDevices != nullptr)
          {
            std::free (pTmpPhysicalDevices);
            pTmpPhysicalDevices = nullptr;
          }

        vkGetPhysicalDeviceProperties2 (pEngine->vulkan.physicalDevice, &physicalDeviceProperties);
#if PATACHE_DEBUG
        vkGetPhysicalDeviceProperties2 (pEngine->vulkan.physicalDevice,
                                        &pEngine->debugInfo.physicalDeviceProperties);
#endif

        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures (pEngine->vulkan.physicalDevice, &deviceFeatures);

        fast_io::io::println (PATACHE_FASTIO_BUFFOUT);

        goto EXIT_CREATE_DEVICE;
      }

    // Search one device from multiple devices
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                          "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found ",
                          gpuCount, " vulkan compatible devices", PATACHE_TERM_RESET);

    std::uint32_t * pGpuScore
        = static_cast<std::uint32_t *> (std::malloc (sizeof (std::uint32_t) * gpuCount));

    if (pGpuScore == nullptr)
      {
        return false;
      }

    VkPhysicalDeviceFeatures2 physicalDeviceFeatures{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = nullptr, .features{}
    };

    for (std::uint32_t i{ 0U }; i < gpuCount; ++i)
      {
        vkGetPhysicalDeviceProperties2 (pTmpPhysicalDevices[i], &physicalDeviceProperties);
        vkGetPhysicalDeviceFeatures2 (pTmpPhysicalDevices[i], &physicalDeviceFeatures);

        switch (physicalDeviceProperties.properties.deviceType)
          {
          case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            pGpuScore[i] += 1000;
            break;
          case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            pGpuScore[i] += 100;
            break;

          case VK_PHYSICAL_DEVICE_TYPE_CPU:
          case VK_PHYSICAL_DEVICE_TYPE_OTHER:
          case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            pGpuScore[i] += 50;
            break;

          case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
            break;
          }

        // Maximum possible texture size.
        pGpuScore[i] += physicalDeviceProperties.properties.limits.maxImageDimension2D;

        fast_io::io::println (
            PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, "  [",
            fast_io::mnp::os_c_str (
                string_VkPhysicalDeviceType (physicalDeviceProperties.properties.deviceType)),
            "] ", PATACHE_TERM_RESET,
            fast_io::mnp::os_c_str (physicalDeviceProperties.properties.deviceName), " ",
            pGpuScore[i]);
      }

    fast_io::io::println (PATACHE_FASTIO_BUFFOUT);

    std::uint32_t highestScore{ 0U }, gpuIndexWinner{ 0U };

    for (std::uint32_t i{ 0U }; i < gpuCount; ++i)
      {
        if (pGpuScore[i] > highestScore)
          {
            highestScore   = pGpuScore[i];
            gpuIndexWinner = i;
          }
      }

    // Select device
    pEngine->vulkan.physicalDevice = pTmpPhysicalDevices[gpuIndexWinner];

    vkGetPhysicalDeviceProperties2 (pEngine->vulkan.physicalDevice, &physicalDeviceProperties);
    pEngine->vulkan.physicalDeviceType = physicalDeviceProperties.properties.deviceType;

#if PATACHE_DEBUG
    pEngine->debugInfo.physicalDeviceProperties.sType
        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    pEngine->debugInfo.physicalDeviceProperties.pNext = nullptr;
    vkGetPhysicalDeviceProperties2 (pEngine->vulkan.physicalDevice,
                                    &pEngine->debugInfo.physicalDeviceProperties);
#endif

    if (pTmpPhysicalDevices != nullptr)
      {
        std::free (pTmpPhysicalDevices);
        pTmpPhysicalDevices = nullptr;
      }

    if (pGpuScore != nullptr)
      {
        std::free (pGpuScore);
        pGpuScore = nullptr;
      }
  }
EXIT_CREATE_DEVICE:

  VmaAllocatorCreateFlags vmaAllocatorInfoFlags{ 0U };

  /*
  Search a queue of graphics from the selected physical device.
  Create a logical device from the selected physical device.
  */
  {
    float queuePriority{ 1.0F };

    // Graphics Queue
    {
      std::uint32_t queueCount{ 0U };

      vkGetPhysicalDeviceQueueFamilyProperties (pEngine->vulkan.physicalDevice, &queueCount,
                                                nullptr);

      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                            "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found ",
                            queueCount, " queues", PATACHE_TERM_RESET);

      VkQueueFamilyProperties * pQueueFamilyProperties{ static_cast<VkQueueFamilyProperties *> (
          std::calloc (queueCount, sizeof (VkQueueFamilyProperties))) };

      if (pQueueFamilyProperties == nullptr)
        {
          return false;
        }

      vkGetPhysicalDeviceQueueFamilyProperties (pEngine->vulkan.physicalDevice, &queueCount,
                                                pQueueFamilyProperties);

      std::uint32_t i{ 0U };
      for (i = 0U; i < queueCount; ++i)
        {
          fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                                fast_io::mnp::right ("Index : ", 10U), PATACHE_TERM_RESET, i);

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_GRAPHICS_BIT", 26U));
            }

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_COMPUTE_BIT", 25U));
            }

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_TRANSFER_BIT", 26U));
            }

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_SPARSE_BINDING_BIT", 32U));
            }

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_PROTECTED_BIT)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_PROTECTED_BIT", 27U));
            }

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_VIDEO_DECODE_BIT_KHR", 34U));
            }

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_VIDEO_ENCODE_BIT_KHR", 34U));
            }

          if (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV)
            {
              fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                                    fast_io::mnp::right ("VK_QUEUE_OPTICAL_FLOW_BIT_NV", 33U));
            }
        }

      for (i = 0U; i < queueCount; ++i)
        {
          if (pQueueFamilyProperties[i].queueFlags
              & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
            {
              pEngine->vulkan.graphicsQueueFamilyIndex = i;

              fast_io::io::println (
                  PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                  "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found index ",
                  pEngine->vulkan.graphicsQueueFamilyIndex,
                  " that contains a (graphics and transfer) queue", PATACHE_TERM_RESET);

              break;
            }
        }

      if (i == UINT32_MAX)
        {
          Patache::FatalErrorMessage ("Patache Engine - Raccoon Renderer",
                                      "No Queue found for graphics", pEngine->configuration);

          return false;
        }

#if PATACHE_DEBUG == 1
      // Copy to debug info
      pEngine->debugInfo.queuePriorityVK = queuePriority;
      pEngine->debugInfo.queueFlagsVK
          = pQueueFamilyProperties[pEngine->vulkan.graphicsQueueFamilyIndex].queueFlags;
#endif

      if (pQueueFamilyProperties != nullptr)
        {
          std::free (pQueueFamilyProperties);
          pQueueFamilyProperties = nullptr;
        }
    }

    // Logical Device
    {
      // Search extensions
      std::uint32_t propertyCount{ 0U };

      // Count
      result = vkEnumerateDeviceExtensionProperties (pEngine->vulkan.physicalDevice, nullptr,
                                                     &propertyCount, nullptr);

      if (result != VK_SUCCESS)
        {
          Patache::FatalErrorMessage (
              "Patache Engine - Raccoon Renderer",
              "vkEnumerateDeviceExtensionProperties() No found device extension count",
              pEngine->configuration);

          return false;
        }

      VkExtensionProperties * pExtensionsProperties{ static_cast<VkExtensionProperties *> (
          std::calloc (propertyCount, sizeof (VkExtensionProperties))) };

      if (pExtensionsProperties == nullptr)
        {
          return false;
        }

      // Device extensions
      result = vkEnumerateDeviceExtensionProperties (pEngine->vulkan.physicalDevice, nullptr,
                                                     &propertyCount, pExtensionsProperties);

      if (result != VK_SUCCESS)
        {
          Patache::FatalErrorMessage (
              "Patache Engine - Raccoon Renderer",
              "vkEnumerateDeviceExtensionProperties() No found device extensions",
              pEngine->configuration);

          return false;
        }

        // Find extensions
#if defined(_WIN64) && defined(VK_USE_PLATFORM_WIN32_KHR)
  #define DEVICE_EXTENSION_COUNT_SEARCH 7 // ++VK_KHR_external_memory_win32
#else
  #define DEVICE_EXTENSION_COUNT_SEARCH 6
#endif

      std::uint8_t deviceExtensionCount{ 1U };
      bool         deviceExtensionIndices[DEVICE_EXTENSION_COUNT_SEARCH]{ false };

#if defined(DEVICE_EXTENSION_COUNT_SEARCH)
  #undef DEVICE_EXTENSION_COUNT_SEARCH
#endif

      for (std::uint32_t i{ 0U }; i < propertyCount; ++i)
        {
          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           VK_KHR_MAINTENANCE_4_EXTENSION_NAME)
              == 0)
            {
              deviceExtensionIndices[0] = true;
              ++deviceExtensionCount;
              continue;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
              == 0)
            {
              deviceExtensionIndices[1] = true;
              ++deviceExtensionCount;
              continue;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           VK_KHR_MAINTENANCE_5_EXTENSION_NAME)
              == 0)
            {
              deviceExtensionIndices[2] = true;
              ++deviceExtensionCount;
              continue;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)
              == 0)
            {
              deviceExtensionIndices[3] = true;
              ++deviceExtensionCount;
              continue;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)
              == 0)
            {
              deviceExtensionIndices[4] = true;
              ++deviceExtensionCount;
              continue;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)
              == 0)
            {
              deviceExtensionIndices[5] = true;
              ++deviceExtensionCount;
              continue;
            }
        }

      // Add device extensions
      std::uint8_t deviceAddExtensionIndexCount{ 0 };

      // malloc
      const char ** ppDeviceExtensions{ static_cast<const char **> (
          std::calloc (deviceExtensionCount, sizeof (const char *))) };

      ppDeviceExtensions[deviceAddExtensionIndexCount] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

      if (deviceExtensionIndices[0])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = VK_KHR_MAINTENANCE_4_EXTENSION_NAME;
          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
        }

      if (deviceExtensionIndices[1])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;

          if (deviceExtensionIndices[2])
            {
              ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
                  = VK_KHR_MAINTENANCE_5_EXTENSION_NAME;
              vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT;
            }
        }

      if (deviceExtensionIndices[3])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = VK_EXT_MEMORY_BUDGET_EXTENSION_NAME;
          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        }

      if (deviceExtensionIndices[4])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME;
          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
        }

      if (deviceExtensionIndices[5])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME;

          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
        }

#if PATACHE_DEBUG == 1
      // Copy to debug info
      pEngine->debugInfo.ppDeviceExtensionsVK
          = static_cast<const char **> (std::calloc (deviceExtensionCount, sizeof (const char *)));

      PATACHE_MEMCPY (pEngine->debugInfo.ppDeviceExtensionsVK, ppDeviceExtensions,
                      sizeof (ppDeviceExtensions) * deviceExtensionCount,
                      sizeof (ppDeviceExtensions) * deviceExtensionCount);

      pEngine->debugInfo.deviceExtensionsCountVK = deviceExtensionCount;
#endif

      fast_io::io::println (PATACHE_FASTIO_BUFFOUT);

      Patache::VulkanList (ppDeviceExtensions, deviceExtensionCount, "Device Extensions");

      // Logical device creation
      const VkDeviceQueueCreateInfo deviceQueueCreateInfo{
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0U,
        .queueFamilyIndex = pEngine->vulkan.graphicsQueueFamilyIndex,
        .queueCount       = 1U,
        .pQueuePriorities = &queuePriority,
      };

      const VkPhysicalDeviceCoherentMemoryFeaturesAMD physicalDeviceCoherentMemoryFeaturesAMDInfo{
        .sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD,
        .pNext                = nullptr,
        .deviceCoherentMemory = (deviceExtensionIndices[5]) ? VK_TRUE : VK_FALSE
      };

      const VkDeviceCreateInfo deviceInfo{ .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                           .pNext = &physicalDeviceCoherentMemoryFeaturesAMDInfo,
                                           .flags = 0U,
                                           .queueCreateInfoCount    = 1U,
                                           .pQueueCreateInfos       = &deviceQueueCreateInfo,
                                           .enabledLayerCount       = 0U,
                                           .ppEnabledLayerNames     = nullptr,
                                           .enabledExtensionCount   = deviceExtensionCount,
                                           .ppEnabledExtensionNames = ppDeviceExtensions,
                                           .pEnabledFeatures        = nullptr };

      result = vkCreateDevice (pEngine->vulkan.physicalDevice, &deviceInfo, nullptr,
                               &pEngine->vulkan.device);

      if (result != VK_SUCCESS)
        {
          Patache::VulkanCheck ("vkCreateDevice()", result);

          Patache::FatalErrorMessage ("Patache Engine - Raccoon Renderer",
                                      "vkCreateDevice() Logical device creation failed",
                                      pEngine->configuration);

          return false;
        }

      // Graphic queue associated with the logic device
      vkGetDeviceQueue (pEngine->vulkan.device, pEngine->vulkan.graphicsQueueFamilyIndex, 0U,
                        &pEngine->vulkan.queue);

      if (ppDeviceExtensions != nullptr)
        {
          std::free (ppDeviceExtensions);
          ppDeviceExtensions = nullptr;
        }

      if (pExtensionsProperties != nullptr)
        {
          std::free (pExtensionsProperties);
          pExtensionsProperties = nullptr;
        }
    }
  }

  // VMA Allocator
  std::future<bool> createAllocator_Async{ std::async (
      std::launch::async,
      [&vmaAllocatorInfoFlags, &pEngine] (void) -> bool
        {
          VmaVulkanFunctions vulkanFunctions{};
          vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
          vulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

          VmaAllocatorCreateInfo vmaAllocatorInfo{ .flags          = vmaAllocatorInfoFlags,
                                                   .physicalDevice = pEngine->vulkan.physicalDevice,
                                                   .device         = pEngine->vulkan.device,
                                                   .preferredLargeHeapBlockSize = 0U,
                                                   .pAllocationCallbacks        = nullptr,
                                                   .pDeviceMemoryCallbacks      = nullptr,
                                                   .pHeapSizeLimit              = nullptr,
                                                   .pVulkanFunctions            = &vulkanFunctions,
                                                   .instance         = pEngine->vulkan.instance,
                                                   .vulkanApiVersion = VK_API_VERSION_1_3,
#if VMA_EXTERNAL_MEMORY
                                                   .pTypeExternalMemoryHandleTypes = nullptr
#endif
          };

          VkResult result{ vmaCreateAllocator (&vmaAllocatorInfo, &pEngine->vulkan.allocator) };

          if (result != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vmaCreateAllocator()", result);

              Patache::FatalErrorMessage ("Patache Engine - Raccoon Renderer",
                                          "vmaCreateAllocator() Allocator creation fail",
                                          pEngine->configuration);

              return false;
            }

          return true;
        }) };

#if PATACHE_DEBUG == 1
  std::future<bool> createImguiDescriptorPool_Async{ std::async (
      std::launch::async, Patache::CreateImguiDescriptorPool, std::ref (pEngine->vulkan)) };
  std::future<bool> createImguiPipelineCache_Async{ std::async (
      std::launch::async, Patache::CreateImguiPipelineCache, std::ref (pEngine->vulkan)) };

  initImguiCore_Async.wait ();
#endif

  // Create a surface for the window to draw on
  if (!SDL_Vulkan_CreateSurface (pEngine->pGameWindow, pEngine->vulkan.instance, nullptr,
                                 &pEngine->vulkan.surface))
    {
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                            "SDL_Vulkan_CreateSurface() : ", PATACHE_TERM_RESET,
                            PATACHE_TERM_COLOR_YELLOW, "Fail", PATACHE_TERM_RESET);

      Patache::FatalErrorMessage ("SDL", SDL_GetError (), pEngine->configuration);

      return false;
    }

  Patache::SwapchainInfo swapchainInfo{};
  if (!CreateSwapchain (pEngine, swapchainInfo))
    return false;

#if PATACHE_DEBUG == 1
  // saving the data for display in imgui
  pEngine->debugInfo.swapchainPresentModeVK      = swapchainInfo.presentMode;
  pEngine->debugInfo.swapchainImageColorFormatVK = swapchainInfo.imageColorFormat;
  pEngine->debugInfo.swapchainImageColorSpaceVK  = swapchainInfo.imageColorSpace;
  {
    VkSurfaceCapabilitiesKHR surfaceCapabilities{};

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR (
        pEngine->vulkan.physicalDevice, pEngine->vulkan.surface, &surfaceCapabilities);

    if (result != VK_SUCCESS)
      {
        Patache::VulkanCheck ("vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result);
      }

    pEngine->debugInfo.minImageCountVK = surfaceCapabilities.minImageCount;
  }
#endif

  if (!Patache::CreateImageView (pEngine->vulkan, swapchainInfo))
    return false;

  /*
  Depthbuffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D
  performance.
  */

  // Render Pass
  {
    const VkAttachmentDescription colorAttachmentDescriptionInfo{
      .flags          = 0U,
      .format         = swapchainInfo.imageColorFormat,
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    constexpr VkAttachmentReference colorAttachmentRef{
      .attachment = 0U,
      .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkSubpassDescription subpassDescriptionInfo{ .flags = 0U,
                                                       .pipelineBindPoint
                                                       = VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                       .inputAttachmentCount = 0U,
                                                       .pInputAttachments    = nullptr,
                                                       .colorAttachmentCount = 1U,
                                                       .pColorAttachments    = &colorAttachmentRef,
                                                       .pResolveAttachments  = nullptr,
                                                       .pDepthStencilAttachment = nullptr,
                                                       .preserveAttachmentCount = 0U,
                                                       .pPreserveAttachments    = nullptr };

    const VkRenderPassCreateInfo renderPassInfo{ .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                                                 .pNext = nullptr,
                                                 .flags = 0U,
                                                 .attachmentCount = 1U,
                                                 .pAttachments    = &colorAttachmentDescriptionInfo,
                                                 .subpassCount    = 1U,
                                                 .pSubpasses      = &subpassDescriptionInfo,
                                                 .dependencyCount = 0U,
                                                 .pDependencies   = nullptr };

    result = vkCreateRenderPass (pEngine->vulkan.device, &renderPassInfo, nullptr,
                                 &pEngine->vulkan.renderPass);

    if (result != VK_SUCCESS)
      {
        Patache::VulkanCheck ("vkCreateRenderPass()", result);

        return false;
      }
  }

  // Command Pool
  std::future<bool> createCommandPool_Async{ std::async (
      std::launch::async,
      [&pEngine] (void) -> bool
        {
          const VkCommandPoolCreateInfo commandPoolInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0U,
            .queueFamilyIndex = pEngine->vulkan.graphicsQueueFamilyIndex,
          };

          pEngine->vulkan.pCommandPools = static_cast<VkCommandPool *> (
              std::malloc (sizeof (VkCommandPool) * pEngine->vulkan.swapchainImageCount));

          if (pEngine->vulkan.pCommandPools == nullptr)
            {
              Patache::FatalErrorMessage (
                  "Raccoon Renderer",
                  "malloc fail to allocate memory heap for vulkan command pools",
                  pEngine->configuration);

              return false;
            }

          VkResult result;

          for (std::uint32_t i{ 0U }; i < pEngine->vulkan.swapchainImageCount; ++i)
            {
              // Render Command Pools
              result = vkCreateCommandPool (pEngine->vulkan.device, &commandPoolInfo, nullptr,
                                            &pEngine->vulkan.pCommandPools[i]);

              if (result != VK_SUCCESS)
                {
                  char errorText[PATACHE_ERROR_TEXT_SIZE]{};

                  std::snprintf (errorText, PATACHE_ERROR_TEXT_SIZE - 1,
                                 "vkCreateCommandPool() Render Command Pool #%.3u", i);

                  Patache::VulkanCheck (errorText, result);

                  return false;
                }
            }

          return true;
        }) };

  std::future<bool> createFrameBuffer_Async{ std::async (
      std::launch::async, Patache::CreateFrameBuffer, std::ref (pEngine->vulkan)) };

  createCommandPool_Async.wait ();
  if (!createCommandPool_Async.get ())
    return false;

  std::future<bool> createCommandBuffer_Async{ std::async (
      std::launch::async, Patache::CreateCommandBuffer, std::ref (pEngine->vulkan)) };

  std::future<bool> createFence_Async{ std::async (std::launch::async, Patache::CreateFence,
                                                   std::ref (pEngine->vulkan)) };

  std::future<bool> createSemaphores_Async{ std::async (
      std::launch::async, Patache::CreateSemaphores, std::ref (pEngine->vulkan)) };

  std::future<bool> createPipeline_Async = std::async (
      std::launch::async,
      [&pEngine] (void) -> bool
        {
          // Load Shaders
          char shaderDirectoryStr[2056]{ 0 };

          PATACHE_STRNCPY (shaderDirectoryStr, SDL_GetBasePath (), 2055, 2056);
          PATACHE_STRNCAT (shaderDirectoryStr, "Shaders/", 2055, 2056);

          fast_io::dir_file shaderDirectory (shaderDirectoryStr);

          fast_io::native_file_loader vertexShader (at (shaderDirectory), "Triangle.vert.spv");

          fast_io::native_file_loader fragmentShader (at (shaderDirectory), "Triangle.frag.spv");

          if (vertexShader.data () == nullptr)
            return false;

          if (fragmentShader.data () == nullptr)
            return false;

          VkShaderModule vertexShaderModule{ VK_NULL_HANDLE },
              fragmentShaderModule{ VK_NULL_HANDLE };

          // Create Shader Modules
          const VkShaderModuleCreateInfo shaderModuleInfo[2]{
            { .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
              .pNext    = nullptr,
              .flags    = 0U,
              .codeSize = vertexShader.size (),
              .pCode    = reinterpret_cast<const std::uint32_t *> (vertexShader.data ()) },
            { .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
              .pNext    = nullptr,
              .flags    = 0U,
              .codeSize = fragmentShader.size (),
              .pCode    = reinterpret_cast<const std::uint32_t *> (fragmentShader.data ()) }
          };

          // Vertex Shader Module
          VkResult result{ vkCreateShaderModule (pEngine->vulkan.device, &shaderModuleInfo[0],
                                                 nullptr, &vertexShaderModule) };

          if (result != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vkCreateShaderModule() Vertex", result);

              return false;
            }

          // Fragment Shader Module
          result = vkCreateShaderModule (pEngine->vulkan.device, &shaderModuleInfo[1], nullptr,
                                         &fragmentShaderModule);

          if (result != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vkCreateShaderModule() Fragment", result);

              return false;
            }

          // Create Pipeline
          const VkPipelineShaderStageCreateInfo shaderStageInfo[2]{
            { .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
              .pNext               = nullptr,
              .flags               = 0U,
              .stage               = VK_SHADER_STAGE_VERTEX_BIT,
              .module              = vertexShaderModule,
              .pName               = "main",
              .pSpecializationInfo = nullptr },
            { .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
              .pNext               = nullptr,
              .flags               = 0U,
              .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
              .module              = fragmentShaderModule,
              .pName               = "main",
              .pSpecializationInfo = nullptr }
          };

          // Vertex Input & Input Assembly
          const VkVertexInputBindingDescription bindingDescription{ .binding = 0U,
                                                                    .stride
                                                                    = sizeof (Patache::Vertex2D),
                                                                    .inputRate
                                                                    = VK_VERTEX_INPUT_RATE_VERTEX };

          const VkVertexInputAttributeDescription vertexInputAttrib[2]{
            // Position
            { .location = 0U,
              .binding  = 0U,
              .format   = VK_FORMAT_R32G32_SFLOAT,
              .offset   = offsetof (Patache::Vertex2D, pos) },
            // Color
            { .location = 1U,
              .binding  = 0U,
              .format   = VK_FORMAT_R32G32B32_SFLOAT,
              .offset   = offsetof (Patache::Vertex2D, color) }
          };

          const VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0U,
            .vertexBindingDescriptionCount   = 1U,
            .pVertexBindingDescriptions      = &bindingDescription,
            .vertexAttributeDescriptionCount = 2U,
            .pVertexAttributeDescriptions    = vertexInputAttrib
          };

          constexpr VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext                  = nullptr,
            .flags                  = 0U,
            .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
          };

          // Dynamic State
          static constexpr VkDynamicState dynamicStates[2U]{ VK_DYNAMIC_STATE_VIEWPORT,
                                                             VK_DYNAMIC_STATE_SCISSOR };

          const VkPipelineDynamicStateCreateInfo dynamicStateInfo{
            .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext             = nullptr,
            .flags             = 0U,
            .dynamicStateCount = 2U,
            .pDynamicStates    = dynamicStates
          };

          // Viewport and Scissor
          pEngine->vulkan.viewport.x = 0U;
          pEngine->vulkan.viewport.y = 0U;

#if PATACHE_LINUX_OR_UNIX
          pEngine->vulkan.viewport.width
              = static_cast<float> (pEngine->vulkan.swapchainExtent.width * scaleInt);
          pEngine->vulkan.viewport.height
              = static_cast<float> (pEngine->vulkan.swapchainExtent.height * scaleInt);
#else
          pEngine->vulkan.viewport.width  = pEngine->vulkan.swapchainExtent.width;
          pEngine->vulkan.viewport.height = pEngine->vulkan.swapchainExtent.height;
#endif

          pEngine->vulkan.viewport.minDepth = 0.0F;
          pEngine->vulkan.viewport.maxDepth = 1.0F;

          pEngine->vulkan.scissor.offset.x = 0U;
          pEngine->vulkan.scissor.offset.y = 0U;

#if PATACHE_LINUX_OR_UNIX
          pEngine->vulkan.scissor.extent.width  = pEngine->vulkan.swapchainExtent.width * scaleInt;
          pEngine->vulkan.scissor.extent.height = pEngine->vulkan.swapchainExtent.height * scaleInt;
#else
          pEngine->vulkan.scissor.extent.width  = pEngine->vulkan.swapchainExtent.width;
          pEngine->vulkan.scissor.extent.height = pEngine->vulkan.swapchainExtent.height;
#endif

          const VkPipelineViewportStateCreateInfo viewportStateInfo{
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext         = nullptr,
            .flags         = 0U,
            .viewportCount = 1U,
            .pViewports    = &pEngine->vulkan.viewport,
            .scissorCount  = 1U,
            .pScissors     = &pEngine->vulkan.scissor
          };

          // Rasterizer
          constexpr const VkPipelineRasterizationStateCreateInfo rasterizerStateInfo{
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0U,
            .depthClampEnable        = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode             = VK_POLYGON_MODE_FILL,
            .cullMode                = VK_CULL_MODE_BACK_BIT,
            .frontFace               = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable         = VK_FALSE,
            .depthBiasConstantFactor = 0.0F,
            .depthBiasClamp          = 0.0F,
            .depthBiasSlopeFactor    = 0.0F,
            .lineWidth               = 1.0F
          };

          // MSAA Multisampling
          const VkPipelineMultisampleStateCreateInfo multisamplingStateInfo{
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = 0U,
            .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable   = VK_FALSE,
            .minSampleShading      = 1.0F,
            .pSampleMask           = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable      = VK_FALSE
          };

          // Depth and stencil testing
          // Not Yet

          // Color Blending
          constexpr const VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
            .blendEnable         = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp        = VK_BLEND_OP_ADD,
            .colorWriteMask      = VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT
                                   | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_A_BIT
          };

          const VkPipelineColorBlendStateCreateInfo colorBlendStateInfo{
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0U,
            .logicOpEnable   = VK_FALSE,
            .logicOp         = VK_LOGIC_OP_COPY,
            .attachmentCount = 1U,
            .pAttachments    = &colorBlendAttachmentState,
            .blendConstants{ 0.0F }
          };

          const VkPipelineLayoutCreateInfo pipelineLayoutInfo{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext                  = nullptr,
            .flags                  = 0U,
            .setLayoutCount         = 0U,
            .pSetLayouts            = nullptr,
            .pushConstantRangeCount = 0U,
            .pPushConstantRanges    = nullptr
          };

          result = vkCreatePipelineLayout (pEngine->vulkan.device, &pipelineLayoutInfo, nullptr,
                                           &pEngine->vulkan.pipelineLayout);
          if (result != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vkCreatePipelineLayout()", result);

              return false;
            }

          const VkGraphicsPipelineCreateInfo graphicsPipelineInfo{
            .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0U,
            .stageCount          = 2U,
            .pStages             = shaderStageInfo,
            .pVertexInputState   = &vertexInputStateInfo,
            .pInputAssemblyState = &inputAssemblyStateInfo,
            .pTessellationState  = nullptr,
            .pViewportState      = &viewportStateInfo,
            .pRasterizationState = &rasterizerStateInfo,
            .pMultisampleState   = &multisamplingStateInfo,
            .pDepthStencilState  = nullptr,
            .pColorBlendState    = &colorBlendStateInfo,
            .pDynamicState       = &dynamicStateInfo,
            .layout              = pEngine->vulkan.pipelineLayout,
            .renderPass          = pEngine->vulkan.renderPass,
            .subpass             = 0U,
            .basePipelineHandle  = VK_NULL_HANDLE,
            .basePipelineIndex   = 0
          };

          result = vkCreateGraphicsPipelines (pEngine->vulkan.device, VK_NULL_HANDLE, 1U,
                                              &graphicsPipelineInfo, nullptr,
                                              &pEngine->vulkan.graphicsPipeline);

          if (result != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vkCreateGraphicsPipelines()", result);

              return false;
            }

          vkDestroyShaderModule (pEngine->vulkan.device, vertexShaderModule, nullptr);
          vkDestroyShaderModule (pEngine->vulkan.device, fragmentShaderModule, nullptr);

          return true;
        });

  std::future<void> vulkanInfo_Async{ std::async (std::launch::async, Patache::VulkanInfo,
                                                  std::ref (pEngine), std::ref (swapchainInfo)) };

#if PATACHE_DEBUG == 1
  createImguiPipelineCache_Async.wait ();
  if (!createImguiPipelineCache_Async.get ())
    return false;

  createImguiDescriptorPool_Async.wait ();
  if (!createImguiDescriptorPool_Async.get ())
    return false;

  Patache::InitImGuiVulkan (pEngine);
#endif

  /*
  CreateDepthBuffer_Async.wait();
  if (!CreateDepthBuffer_Async.get()) return false;
  */

  createFrameBuffer_Async.wait ();
  if (!createFrameBuffer_Async.get ())
    return false;

  createCommandBuffer_Async.wait ();
  if (!createCommandBuffer_Async.get ())
    return false;

  createAllocator_Async.wait ();
  if (!createAllocator_Async.get ())
    return false;

  std::future<bool> createBuffer_Async{ std::async (
      std::launch::async,
      [&pEngine, &physicalDeviceProperties, &rInfo] (void) -> bool
        {
          if (rInfo.buffStagingSize < 262144U)
            {
              Patache::FatalErrorMessage ("Patache Engine - Raccoon Renderer",
                                          "buffStagingSize cannot be < (262144 bytes)",
                                          pEngine->configuration);

              return false;
            }

          pEngine->vulkan.stagingBufferInfo[Patache::VkBufferInfo::eSize] = rInfo.buffStagingSize;

          // GPU-CPU visible transfer buffer
          const VkBufferCreateInfo buffStagingInfo{
            .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext       = nullptr,
            .flags       = 0U,
            .size        = rInfo.buffStagingSize,
            .usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1U,
            .pQueueFamilyIndices   = &pEngine->vulkan.graphicsQueueFamilyIndex
          };

          constexpr VmaAllocationCreateInfo allocInfo{
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
                     | VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
            .requiredFlags
            = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
            .memoryTypeBits = 0U,
            .pool           = nullptr,
            .pUserData      = nullptr,
            .priority       = 1.0F
          };

          VkResult result{ vmaCreateBuffer (pEngine->vulkan.allocator, &buffStagingInfo, &allocInfo,
                                            &pEngine->vulkan.stagingBuffer,
                                            &pEngine->vulkan.stagingAllocation, nullptr) };

          if (result != VK_SUCCESS)
            {
              Patache::VulkanCheck ("vmaCreateBuffer()", result);

              return false;
            }

          // GPU-device
          if (physicalDeviceProperties.properties.deviceType
              == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
              if (rInfo.memRenderSizePerImage < 262144U)
                {
                  Patache::FatalErrorMessage ("Patache Engine - Raccoon Renderer",
                                              "memRenderSizePerImage cannot be < (262144 bytes)",
                                              pEngine->configuration);

                  return false;
                }

              pEngine->vulkan.renderBufferInfo[Patache::VkBufferInfo::eSize]
                  = rInfo.memRenderSizePerImage;

              const VkBufferCreateInfo buffRenderInfo{
                .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .pNext       = nullptr,
                .flags       = 0U,
                .size        = rInfo.memRenderSizePerImage,
                .usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 1U,
                .pQueueFamilyIndices   = &pEngine->vulkan.graphicsQueueFamilyIndex
              };

              const VmaAllocationCreateInfo allocInfo{
                .flags          = VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT
                                  | VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT,
                .usage          = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                .requiredFlags  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD,
                .memoryTypeBits = 0U,
                .pool           = pEngine->vulkan.renderPool,
                .pUserData      = nullptr,
                .priority       = 1.0F
              };

              // Memory pool
              std::uint32_t memoryTypeIndex{ 0U };

              result = vmaFindMemoryTypeIndexForBufferInfo (
                  pEngine->vulkan.allocator, &buffRenderInfo, &allocInfo, &memoryTypeIndex);

              if (result != VK_SUCCESS)
                {
                  Patache::VulkanCheck ("vmaFindMemoryTypeIndexForBufferInfo()", result);

                  return false;
                }

              const VmaPoolCreateInfo poolCreateInfo{ .memoryTypeIndex        = memoryTypeIndex,
                                                      .flags                  = 0U,
                                                      .blockSize              = 0U,
                                                      .minBlockCount          = 0U,
                                                      .maxBlockCount          = 0U,
                                                      .priority               = 1.0F,
                                                      .minAllocationAlignment = 0U,
                                                      .pMemoryAllocateNext    = nullptr };

              result = vmaCreatePool (pEngine->vulkan.allocator, &poolCreateInfo,
                                      &pEngine->vulkan.renderPool);

              if (result != VK_SUCCESS)
                {
                  Patache::VulkanCheck ("vmaCreatePool()", result);

                  return false;
                }

              // Device memory
              pEngine->vulkan.pRenderBuffer = static_cast<VkBuffer *> (
                  std::calloc (pEngine->vulkan.swapchainImageCount, sizeof (VkBuffer)));

              pEngine->vulkan.pRenderAllocation = static_cast<VmaAllocation *> (
                  std::calloc (pEngine->vulkan.swapchainImageCount, sizeof (VmaAllocation)));

              if (pEngine->vulkan.pRenderBuffer == nullptr
                  || pEngine->vulkan.pRenderAllocation == nullptr)
                {
                  Patache::ErrorMessage ("Cant allocate memory");

                  return false;
                }

              for (std::uint32_t i{ 0U }; i < pEngine->vulkan.swapchainImageCount; ++i)
                {
                  result = vmaCreateBuffer (pEngine->vulkan.allocator, &buffRenderInfo, &allocInfo,
                                            &pEngine->vulkan.pRenderBuffer[i],
                                            &pEngine->vulkan.pRenderAllocation[i], nullptr);

                  if (result != VK_SUCCESS)
                    {
                      Patache::VulkanCheck ("vmaCreateBuffer()", result);

                      return false;
                    }
                }
            }

          return true;
        }) };

  createFence_Async.wait ();
  if (!createFence_Async.get ())
    return false;

  createSemaphores_Async.wait ();
  if (!createSemaphores_Async.get ())
    return false;

  createPipeline_Async.wait ();
  if (!createPipeline_Async.get ())
    return false;

  createBuffer_Async.wait ();
  if (!createBuffer_Async.get ())
    return false;

  return true;
}

#if PATACHE_DEBUG == 1
  #include <imgui_impl_vulkan.h>
#endif

void
Patache::RaccoonRendererClose (Patache::VulkanBackend & rVulkan)
{
  if (rVulkan.device == VK_NULL_HANDLE || rVulkan.instance == VK_NULL_HANDLE)
    return;

  std::uint_fast32_t i{ 0U };

  VkResult result{ vkDeviceWaitIdle (rVulkan.device) };

  if (result != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkDeviceWaitIdle()", result);
    }

  if (rVulkan.queue == VK_NULL_HANDLE)
    return;

  result = vkQueueWaitIdle (rVulkan.queue);

  if (result != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkQueueWaitIdle()", result);
    }

  // Swapchain
  // Color
  for (i = 0; i < rVulkan.swapchainImageCount; ++i)
    {
      if (rVulkan.pSwapchainFrameBuffers != nullptr)
        {
          vkDestroyFramebuffer (rVulkan.device, rVulkan.pSwapchainFrameBuffers[i], nullptr);
        }

      if (rVulkan.pSwapchainColorImageViews != nullptr)
        {
          vkDestroyImageView (rVulkan.device, rVulkan.pSwapchainColorImageViews[i], nullptr);
        }
    }

  if (rVulkan.pSwapchainFrameBuffers != nullptr)
    {
      std::free (rVulkan.pSwapchainFrameBuffers);
      rVulkan.pSwapchainFrameBuffers = nullptr;
    }

  if (rVulkan.pSwapchainColorImageViews != nullptr)
    {
      std::free (rVulkan.pSwapchainColorImageViews);
      rVulkan.pSwapchainColorImageViews = nullptr;
    }

  if (rVulkan.pSwapchainImages != nullptr)
    {
      std::free (rVulkan.pSwapchainImages);
      rVulkan.pSwapchainImages = nullptr;
    }

  vkDestroySwapchainKHR (rVulkan.device, rVulkan.swapchain, nullptr);

  // Buffer
  for (i = 0; i < rVulkan.swapchainImageCount; ++i)
    {
      if (rVulkan.pRenderBuffer[i] != nullptr && rVulkan.pRenderAllocation[i] != nullptr)
        {
          vmaDestroyBuffer (rVulkan.allocator, rVulkan.pRenderBuffer[i],
                            rVulkan.pRenderAllocation[i]);
        }
    }

  if (rVulkan.pRenderBuffer != nullptr)
    {
      std::free (rVulkan.pRenderBuffer);
      rVulkan.pRenderBuffer = nullptr;
    }

  if (rVulkan.pRenderAllocation != nullptr)
    {
      std::free (rVulkan.pRenderAllocation);
      rVulkan.pRenderAllocation = nullptr;
    }

  if (rVulkan.stagingBuffer != VK_NULL_HANDLE)
    {
      vmaDestroyBuffer (rVulkan.allocator, rVulkan.stagingBuffer, rVulkan.stagingAllocation);
    }

  if (rVulkan.renderPool != nullptr)
    {
      vmaDestroyPool (rVulkan.allocator, rVulkan.renderPool);
    }

#if PATACHE_DEBUG == 1
  // ImGui
  ImGuiIO * pIO{ nullptr };

  if (ImGui::GetCurrentContext () != nullptr)
    {
      pIO = &ImGui::GetIO ();

      if (pIO->BackendPlatformName != nullptr)
        {
          ImGui_ImplVulkan_Shutdown ();
        }
    }

  vkDestroyDescriptorPool (rVulkan.device, rVulkan.imguiDescriptorPool, nullptr);
  vkDestroyPipelineCache (rVulkan.device, rVulkan.imguiPipelineCache, nullptr);
#endif

  // Pipeline
  vkDestroyPipelineCache (rVulkan.device, rVulkan.graphicsPipelineCache, nullptr);
  vkDestroyPipelineLayout (rVulkan.device, rVulkan.pipelineLayout, nullptr);
  vkDestroyPipeline (rVulkan.device, rVulkan.graphicsPipeline, nullptr);

  vkDestroyRenderPass (rVulkan.device, rVulkan.renderPass, nullptr);

  // Primitivas de sincronizacion
  for (i = 0; i < rVulkan.swapchainImageCount; ++i)
    {
      vkDestroySemaphore (rVulkan.device, rVulkan.pImageAvailableSemaphores[i], nullptr);
      vkDestroySemaphore (rVulkan.device, rVulkan.pImageFinishedSemaphores[i], nullptr);
      vkDestroyFence (rVulkan.device, rVulkan.pInFlightFences[i], nullptr);
    }

  if (rVulkan.pImageAvailableSemaphores != nullptr)
    {
      std::free (rVulkan.pImageAvailableSemaphores);
      rVulkan.pImageAvailableSemaphores = nullptr;
    }

  if (rVulkan.pImageFinishedSemaphores != nullptr)
    {
      std::free (rVulkan.pImageFinishedSemaphores);
      rVulkan.pImageFinishedSemaphores = nullptr;
    }

  if (rVulkan.pInFlightFences != nullptr)
    {
      std::free (rVulkan.pInFlightFences);
      rVulkan.pInFlightFences = nullptr;
    }

  // Command Pools and Buffers
  if (rVulkan.pCommandPools != nullptr)
    {
      for (i = 0; i < rVulkan.swapchainImageCount; ++i)
        {
          vkDestroyCommandPool (rVulkan.device, rVulkan.pCommandPools[i], nullptr);
        }

      std::free (rVulkan.pCommandPools);
      rVulkan.pCommandPools = nullptr;
    }

  if (rVulkan.pCmd != nullptr)
    {
      std::free (rVulkan.pCmd);
      rVulkan.pCmd = nullptr;
    }

  vmaDestroyAllocator (rVulkan.allocator);

  vkDestroyDevice (rVulkan.device, nullptr);

  vkDestroySurfaceKHR (rVulkan.instance, rVulkan.surface, nullptr);

#if PATACHE_DEBUG == 1
  if (pfnVkCreateDebugUtilsMessengerEXT != nullptr)
    {
      pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT> (
          vkGetInstanceProcAddr (rVulkan.instance, "vkDestroyDebugUtilsMessengerEXT"));

      if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr)
        {
          Patache::ErrorMessage ("vkGetInstanceProcAddr() Cannot get address of function "
                                 "vkDestroyDebugUtilsMessengerEXT()");

          return;
        }

      vkDestroyDebugUtilsMessengerEXT (rVulkan.instance, rVulkan.debugMessenger, nullptr);
    }
#endif

  vkDestroyInstance (rVulkan.instance, nullptr);
}
