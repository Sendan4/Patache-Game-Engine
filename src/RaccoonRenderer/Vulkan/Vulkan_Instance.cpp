#include "Vulkan_Instance.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateInstance (
    SDL_Window *& WINDOW, YAML::Node & CONFIG)
{
  vk::ApplicationInfo PatataEngineInfo (
      PATATA_GAME_NAME, PATATA_GAME_VERSION, PATATA_ENGINE_NAME,
      PATATA_ENGINE_VERSION_VK, VK_API_VERSION_1_3);

  /*
  Las capas de validacion se activan con USE_VVL=ON.
  Son para el desarrollo y prueba de este backend.
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
  uint32_t SDLExtensionCount = 0, TotalExtensionCount = 1;

  SDL_Vulkan_GetInstanceExtensions (WINDOW, &SDLExtensionCount,
                                    nullptr);

  TotalExtensionCount += SDLExtensionCount;

  const char ** pExtensionInstanceNames
      = new const char *[TotalExtensionCount];

  bool FoundExtensions = SDL_Vulkan_GetInstanceExtensions (
      WINDOW, &SDLExtensionCount, pExtensionInstanceNames);

  // From here the other extensions should be added, from the count returned by SDL onwards.
  pExtensionInstanceNames[SDLExtensionCount] = "VK_KHR_get_surface_capabilities2";

  if (FoundExtensions)
	  std::future<void> ReturnVulkanList = std::async(std::launch::async, Patata::Log::VulkanList, pExtensionInstanceNames, TotalExtensionCount - 1, "Instance Extensions");

  // Create Instance
  vk::InstanceCreateInfo InstanceInfo ({}, &PatataEngineInfo,
#if defined(DEBUG) && defined(PATATA_USE_VVL)
                                       1, &layer,
#else
                                       0, nullptr,
#endif
                                       TotalExtensionCount,
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
