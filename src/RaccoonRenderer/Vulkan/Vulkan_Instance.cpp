#include "Vulkan_Instance.hpp"

bool
Patata::RaccoonRenderer::CreateInstance (void)
{
  vk::ApplicationInfo PatataEngineInfo (
      PATATA_GAME_NAME,         // Application Name
      PATATA_GAME_VERSION,      // Application Version
      PATATA_ENGINE_NAME,       // Engine Name
      PATATA_ENGINE_VERSION_VK, // Engine Version
      VK_API_VERSION_1_3        // Vulkan API Version
  );

/*
The validation layers are activated with USE_VVL=ON.
They are for the development and testing of this backend.
*/
// Layers
#if defined(DEBUG) && defined(PATATA_USE_VVL)
  const char * pLayer[PATATA_VK_LAYER_COUNT]{
    "VK_LAYER_KHRONOS_validation",
  };
  {
    std::future<void> ReturnVulkanList
        = std::async (std::launch::async, Patata::Log::VulkanList, pLayer,
                      PATATA_VK_LAYER_COUNT, "Layers");
  }
  for (uint16_t i = 0; i < PATATA_VK_LAYER_COUNT; ++i)
    pRaccoonInfo->pPatataEngineInfo->VkLayers[i] = pLayer[i];
#endif

  // Get Extensions
  uint32_t SDLExtensionCount = 0;

  SDL_Vulkan_GetInstanceExtensions (*pRaccoonInfo->ppWindow,
                                    &SDLExtensionCount, nullptr);

#if defined(DEBUG)
  uint32_t      MyExtensionCount        = SDLExtensionCount + 1;
  const char ** pExtensionInstanceNames = new const char *[MyExtensionCount];
#else
  const char ** pExtensionInstanceNames = new const char *[SDLExtensionCount];
#endif

  bool FoundExtensions = SDL_Vulkan_GetInstanceExtensions (
      *pRaccoonInfo->ppWindow, &SDLExtensionCount, pExtensionInstanceNames);

#if defined(DEBUG)
  pExtensionInstanceNames[SDLExtensionCount] = "VK_EXT_debug_utils";
#endif

  if (FoundExtensions)
    std::future<void> VulkanList = std::async (
        std::launch::async, Patata::Log::VulkanList, pExtensionInstanceNames,
#if defined(DEBUG)
        MyExtensionCount,
#else
        SDLExtensionCount,
#endif
        "Instance Extensions");

#if defined(DEBUG)
  pRaccoonInfo->pPatataEngineInfo->VkInstanceExtensions
      = new const char *[MyExtensionCount];

  for (uint32_t i = 0; i < MyExtensionCount; ++i)
    pRaccoonInfo->pPatataEngineInfo->VkInstanceExtensions[i]
        = pExtensionInstanceNames[i];

  pRaccoonInfo->pPatataEngineInfo->VkInstanceExtensionsCount
      = MyExtensionCount;
#endif

  // Create Instance
  vk::InstanceCreateInfo InstanceInfo (
      {},
      &PatataEngineInfo, // pApplicationInfo
#if defined(DEBUG) && defined(PATATA_USE_VVL)
      // Using Layers
      PATATA_VK_LAYER_COUNT, // enabledLayerCount
      pLayer,                // ppEnabledLayerNames
#else
      // Not Using Layers
      0,       // enabledLayerCount
      nullptr, // ppEnabledLayerNames
#endif
#if defined(DEBUG)
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

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Instance", Result);

      std::future<void> ReturnVulkanErr = std::async (
          std::launch::async, Patata::Log::FatalErrorMessage,
          "Patata Engine - Raccoon Renderer",
          std::string ("You do not have Vulkan API compatible drivers or your "
                       "GPU does not support the Vulkan API. "
                       + vk::to_string (Result)),
          *pRaccoonInfo->pConfiguration);

      return false;
    }
  else
    return true;
}
