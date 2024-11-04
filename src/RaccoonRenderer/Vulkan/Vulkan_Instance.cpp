#include "Vulkan_Instance.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateInstance (void)
{
  vk::ApplicationInfo PatataEngineInfo (
      PATATA_GAME_NAME,         // Application Name
      PATATA_GAME_VERSION,      // Application Version
      PATATA_ENGINE_NAME,       // Engine Name
      PATATA_ENGINE_VERSION_VK, // Engine Version
      VK_API_VERSION_1_3        // Vulkan API Version
  );

  /*
  The validation layers are activated with USE_VVVL=ON.
  They are for the development and testing of this backend.
  */
  // Layers
  #if defined(DEBUG) && defined(PATATA_USE_VVL)
  const char * layer{ "VK_LAYER_KHRONOS_validation" };
  {
    std::future<void> ReturnVulkanList = std::async(
        std::launch::async, Patata::Log::VulkanList, &layer, 0, "Layers");
  }
  #endif

  //Get Extensions
  uint32_t SDLExtensionCount = 0;

  SDL_Vulkan_GetInstanceExtensions (*pRaccoonInfo->ppWindow, &SDLExtensionCount,
                                    nullptr);

  const char ** pExtensionInstanceNames = new const char * [SDLExtensionCount];

  bool FoundExtensions = SDL_Vulkan_GetInstanceExtensions (
      *pRaccoonInfo->ppWindow, &SDLExtensionCount, pExtensionInstanceNames);

  if (FoundExtensions)
    std::future<void> VulkanList = std::async (
        std::launch::async, Patata::Log::VulkanList,
        pExtensionInstanceNames, SDLExtensionCount - 1, "Instance Extensions");

  #if defined (DEBUG)
  pRaccoonInfo->pPatataEngineInfo->VkInstanceExtensions = new const char * [SDLExtensionCount];

  for (uint32_t i = 0; i < SDLExtensionCount; ++i)
      pRaccoonInfo->pPatataEngineInfo->VkInstanceExtensions[i] = pExtensionInstanceNames[i];

  pRaccoonInfo->pPatataEngineInfo->VkInstanceExtensionsCount = SDLExtensionCount;
  #endif

  // Create Instance
  vk::InstanceCreateInfo InstanceInfo ({},
    &PatataEngineInfo,        // pApplicationInfo
    #if defined(DEBUG) && defined(PATATA_USE_VVL)
    // Using Layers
    1,                        // enabledLayerCount
    &layer,                   // ppEnabledLayerNames
    #else
    // Not Using Layers
    0,                        // enabledLayerCount
    nullptr,                  // ppEnabledLayerNames
    #endif
    SDLExtensionCount,        // enabledExtensionCount
    pExtensionInstanceNames,  // ppEnabledExtensionNames
    nullptr                   // pNext
  );

  vk::Result Result = vk::createInstance (&InstanceInfo, nullptr, &Instance);

  {
      std::future<void> ReturnVulkanCheck = std::async(
          std::launch::async, Patata::Log::VulkanCheck, "Instance", Result);
  }

  delete[] pExtensionInstanceNames;

  if (Result != vk::Result::eSuccess) {
      std::future<void> ReturnVulkanErr = std::async(std::launch::async,
             Patata::Log::FatalErrorMessage,
             "Patata Engine - Raccoon Renderer",
             std::string("You do not have Vulkan API compatible drivers or your GPU does not support the Vulkan API. ")
             + vk::to_string (Result),
             *pRaccoonInfo->pConfiguration);

      return false;
  }
  else return true;
}
