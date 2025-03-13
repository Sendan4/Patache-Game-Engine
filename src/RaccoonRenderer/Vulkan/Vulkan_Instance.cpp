#include "Vulkan_Instance.hpp"

bool
Patata::Engine::CreateInstance (const Patata::EngineCreateInfo & Info)
{
  const char * ApplicationName
      = (Info.gameName != nullptr) ? Info.gameName : "Untitled Game";

  std::uint32_t GameVersion = Info.gameVersion;

  vk::ApplicationInfo PatataEngineInfo{ .pApplicationName   = ApplicationName,
                                        .applicationVersion = GameVersion,
                                        .pEngineName = PATATA_ENGINE_NAME,
                                        .engineVersion
                                        = PATATA_ENGINE_VERSION_VK,
                                        .apiVersion = VK_API_VERSION_1_3 };

// The validation layers are activated with USE_VVL=ON.
// They are for the development and testing of this backend.
// Layers
#if PATATA_DEBUG == 1 && defined(PATATA_USE_VVL)
  engineInfo.ppVkLayers    = new const char *[PATATA_VK_LAYER_COUNT];
  engineInfo.ppVkLayers[0] = "VK_LAYER_KHRONOS_validation";

  // List Layers
  {
    std::future<void> ReturnVulkanList
        = std::async (std::launch::async, Patata::Log::VulkanList,
                      engineInfo.ppVkLayers, PATATA_VK_LAYER_COUNT, "Layers");
  }
#endif

  // Get Extensions
  uint32_t SDLExtensionCount = 0;

  SDL_Vulkan_GetInstanceExtensions (GameWindow, &SDLExtensionCount, nullptr);

#if PATATA_DEBUG == 1
#if defined(PATATA_USE_VVL)
  engineInfo.VkInstanceExtensionsCount = SDLExtensionCount + 2;
#else
  engineInfo.VkInstanceExtensionsCount = SDLExtensionCount + 1;
#endif

#define EXTENSION_COUNT engineInfo.VkInstanceExtensionsCount // Debug
#else                                                        // PATATA_DEBUG
#define EXTENSION_COUNT SDLExtensionCount                    // Release
#endif                                                       // PATATA_DEBUG

  const char ** ppExtensionInstanceNames = new const char *[EXTENSION_COUNT];

  if (SDL_Vulkan_GetInstanceExtensions (GameWindow, &SDLExtensionCount,
                                        ppExtensionInstanceNames))
    {
#if PATATA_DEBUG == 1
      ppExtensionInstanceNames[SDLExtensionCount]
          = vk::EXTDebugUtilsExtensionName;
#if defined(PATATA_USE_VVL)
      ppExtensionInstanceNames[SDLExtensionCount + 1]
          = vk::EXTLayerSettingsExtensionName;
#endif

      // Struct / Patata Engine Info
      engineInfo.ppVkInstanceExtensions = new const char *[EXTENSION_COUNT];

      for (std::uint8_t i = 0; i < EXTENSION_COUNT; ++i)
        engineInfo.ppVkInstanceExtensions[i] = ppExtensionInstanceNames[i];
#endif

      std::future<void> VulkanList = std::async (
          std::launch::async, Patata::Log::VulkanList,
          ppExtensionInstanceNames, EXTENSION_COUNT, "Instance Extensions");
    }
  else
    {
      std::future<void> FatalErr = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer",
          "Cannot find vulkan surface extensions", configuration);

      return false;
    }

    // Configuration of Validation Layers
#if PATATA_DEBUG == 1 && defined(PATATA_USE_VVL)
  static constexpr const vk::Bool32 EnableSetting = VK_TRUE;

  constexpr const vk::LayerSettingEXT Settings[]
      = { {
              .pLayerName   = "VK_LAYER_KHRONOS_validation",
              .pSettingName = "thread_safety",
              .type         = vk::LayerSettingTypeEXT::eBool32,
              .valueCount   = 1,
              .pValues      = &EnableSetting,
          },
          {
              .pLayerName   = "VK_LAYER_KHRONOS_validation",
              .pSettingName = "validate_sync",
              .type         = vk::LayerSettingTypeEXT::eBool32,
              .valueCount   = 1,
              .pValues      = &EnableSetting,
          },
          {
              .pLayerName   = "VK_LAYER_KHRONOS_validation",
              .pSettingName = "validate_best_practices",
              .type         = vk::LayerSettingTypeEXT::eBool32,
              .valueCount   = 1,
              .pValues      = &EnableSetting,
          } };

  const vk::LayerSettingsCreateInfoEXT LayerSettingsInfo{ .settingCount = 3,
                                                          .pSettings
                                                          = Settings };

#define EXTENT_STRUCT &LayerSettingsInfo // Pointer to struct
#else
#define EXTENT_STRUCT nullptr
#endif

  // Create Instance
  vk::InstanceCreateInfo InstanceInfo{
    .pNext            = EXTENT_STRUCT,
    .pApplicationInfo = &PatataEngineInfo,
#if PATATA_DEBUG == 1 && defined(PATATA_USE_VVL)
    // Using Layers
    .enabledLayerCount   = PATATA_VK_LAYER_COUNT,
    .ppEnabledLayerNames = engineInfo.ppVkLayers,
#else
    // Not Using Layers
    .enabledLayerCount   = 0,
    .ppEnabledLayerNames = nullptr,
#endif
    .enabledExtensionCount   = EXTENSION_COUNT,
    .ppEnabledExtensionNames = ppExtensionInstanceNames,
  };

  vk::Result Result
      = vk::createInstance (&InstanceInfo, nullptr, &Vulkan.Instance);

  delete[] ppExtensionInstanceNames;

  if (Result == vk::Result::eErrorIncompatibleDriver)
    {
      char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

      PATATA_STRNCPY (ErrorText,
                      "You do not have Vulkan API compatible drivers or your "
                      "GPU does not support the Vulkan API. ",
                      PATATA_ERROR_TEXT_SIZE - 1);

      PATATA_STRNCAT (ErrorText, vk::to_string (Result).c_str (),
                      PATATA_ERROR_TEXT_SIZE - 1);

      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer", ErrorText, configuration);

      return false;
    }

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Instance", Result);

      return false;
    }

    // Debug Utils Messenger Info
#if PATATA_DEBUG == 1
  // Get function address
  pfnVkCreateDebugUtilsMessengerEXT
      = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT> (
          Vulkan.Instance.getProcAddr ("vkCreateDebugUtilsMessengerEXT"));

  if (pfnVkCreateDebugUtilsMessengerEXT == nullptr)
    return false;

  // Debug Messenger Info
  vk::DebugUtilsMessengerCreateInfoEXT DebugMessengerInfo{
    .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                       | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
    .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                   | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                   | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
    .pfnUserCallback = &debugMessageFunc
  };

  Result = Vulkan.Instance.createDebugUtilsMessengerEXT (
      &DebugMessengerInfo, nullptr, &Vulkan.DebugMessenger);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Debug Utils Messenger", Result);

      return false;
    }
#endif

  return true;
}
