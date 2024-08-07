#include "Vulkan_Instance.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateInstance (
    SDL_Window *& WINDOW, YAML::Node & CONFIG)
{
  vk::ApplicationInfo PatataEngineInfo (
      PATATA_GAME_NAME, PATATA_GAME_VERSION, PATATA_ENGINE_NAME,
      PATATA_ENGINE_VERSION_VK, VK_API_VERSION_1_3);

  /*
  The validation layers are activated with USE_VVVL=ON.
  They are for the development and testing of this backend.
  */
  // Layers
#if defined(DEBUG) && defined(PATATA_USE_VVL)
  const char * layer{ "VK_LAYER_KHRONOS_validation" };
  {
    std::future<void> ReturnVulkanList = std::async(std::launch::async, Patata::Log::VulkanList, &layer, 0, "Layers");
  }
#endif

  /*
  Get Extensions

  if you want to add an extension, take into account TotalExtensionCount
  */
  uint32_t SDLExtensionCount = 0;

  SDL_Vulkan_GetInstanceExtensions (WINDOW, &SDLExtensionCount,
                                    nullptr);

  const char ** pExtensionInstanceNames = new const char *[SDLExtensionCount];

  bool FoundExtensions = SDL_Vulkan_GetInstanceExtensions (
      WINDOW, &SDLExtensionCount, pExtensionInstanceNames);



  if (FoundExtensions)
    std::future<void> ReturnVulkanList = std::async (
        std::launch::async, Patata::Log::VulkanList, pExtensionInstanceNames,
        SDLExtensionCount - 1, "Instance Extensions");

  // Create Instance
  vk::InstanceCreateInfo InstanceInfo ({}, &PatataEngineInfo,
#if defined(DEBUG) && defined(PATATA_USE_VVL)
                                       1, &layer,
#else
                                       0, nullptr,
#endif
                                       SDLExtensionCount,
                                       pExtensionInstanceNames);

  vk::Result Result;
  try
    {
      Result = vk::createInstance (&InstanceInfo, nullptr, &Instance);
	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Instance", Result);
    }
  catch (const vk::Result & Error)
    {
      Patata::Log::FatalErrorMessage ("Vulkan Error", vk::to_string (Error),
                                      CONFIG);
      return false;
    }

  delete[] pExtensionInstanceNames;

  return true;
}
