#include "Vulkan_Instance.hpp"

bool
Patata::Engine::CreateInstance (const Patata::EngineCreateInfo & Info)
{
  const char * ApplicationName
      = (Info.gameName != nullptr) ? Info.gameName : "Untitled Game";

  uint32_t GameVersion = Info.gameVersion;

  vk::ApplicationInfo PatataEngineInfo (
      ApplicationName,          // Application Name
      GameVersion,              // Application Version
      PATATA_ENGINE_NAME,       // Engine Name
      PATATA_ENGINE_VERSION_VK, // Engine Version
      VK_API_VERSION_1_3        // Vulkan API Version
  );

/*
The validation layers are activated with USE_VVL=ON.
They are for the development and testing of this backend.
*/
// Layers
#if PATATA_DEBUG == 1 && defined(PATATA_USE_VVL)
  engineInfo.ppVkLayers    = new const char *[PATATA_VK_LAYER_COUNT];
  engineInfo.ppVkLayers[0] = "VK_LAYER_KHRONOS_validation";

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
  uint32_t      MyExtensionCount        = SDLExtensionCount + 1;
  const char ** pExtensionInstanceNames = new const char *[MyExtensionCount];
#else
  const char ** pExtensionInstanceNames = new const char *[SDLExtensionCount];
#endif

  bool FoundExtensions = SDL_Vulkan_GetInstanceExtensions (
      GameWindow, &SDLExtensionCount, pExtensionInstanceNames);

#if PATATA_DEBUG == 1
  pExtensionInstanceNames[SDLExtensionCount] = "VK_EXT_debug_utils";
#endif

  if (FoundExtensions)
    std::future<void> VulkanList = std::async (
        std::launch::async, Patata::Log::VulkanList, pExtensionInstanceNames,
#if PATATA_DEBUG == 1
        MyExtensionCount,
#else
        SDLExtensionCount,
#endif
        "Instance Extensions");

  // Struct / Patata Engine Info
#if PATATA_DEBUG == 1
  engineInfo.ppVkInstanceExtensions = new const char *[MyExtensionCount];

  for (uint8_t i = 0; i < MyExtensionCount; ++i)
    engineInfo.ppVkInstanceExtensions[i] = pExtensionInstanceNames[i];

  engineInfo.VkInstanceExtensionsCount = MyExtensionCount;
#endif

  // Create Instance
  vk::InstanceCreateInfo InstanceInfo (
      {},
      &PatataEngineInfo, // pApplicationInfo
#if PATATA_DEBUG == 1 && defined(PATATA_USE_VVL)
      // Using Layers
      PATATA_VK_LAYER_COUNT, // enabledLayerCount
      engineInfo.ppVkLayers, // ppEnabledLayerNames
#else
      // Not Using Layers
      0,       // enabledLayerCount
      nullptr, // ppEnabledLayerNames
#endif
#if PATATA_DEBUG == 1
      MyExtensionCount, // enabledExtensionCount
#else
      SDLExtensionCount, // enabledExtensionCount
#endif
      pExtensionInstanceNames, // ppEnabledExtensionNames
      nullptr                  // pNext
  );

  vk::Result Result
      = vk::createInstance (&InstanceInfo, nullptr, &Vulkan.Instance);

  delete[] pExtensionInstanceNames;

  if (Result == vk::Result::eErrorIncompatibleDriver)
    {
      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer",
          std::string_view{
              "You do not have Vulkan API compatible drivers or your "
              "GPU does not support the Vulkan API. "
              + vk::to_string (Result) },
          configuration);

      return false;
    }

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Instance", Result);

      return false;
    }
  else
    return true;
}
