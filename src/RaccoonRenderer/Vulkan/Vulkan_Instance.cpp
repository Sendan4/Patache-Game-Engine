#include "Vulkan_Instance.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateInstance (
    SDL_Window *& WINDOW, YAML::Node & CONFIG)
{
  vk::ApplicationInfo PatataEngineInfo (
      PATATA_GAME_NAME, PATATA_GAME_VERSION, PATATA_ENGINE_NAME,
      PATATA_ENGINE_VERSION_VK, VK_API_VERSION_1_3);

// Layers
#if defined(DEBUG) && defined(PATATA_USE_VVL)
  const char * layer{ "VK_LAYER_KHRONOS_validation" };
  {
    std::future<void> ReturnVulkanList = std::async(std::launch::async, Patata::Log::VulkanList, &layer, 0, "Layers");
  }
#endif

  // Get Extensions
  // Array Index 0 "SDL Extensions". Array Index 1 "My Extensions"
  // El conteo comienza desde el 1 con las extensiones.
  uint32_t extensionInstanceCount[2]{ 0, 1 };

  SDL_Vulkan_GetInstanceExtensions (WINDOW, &extensionInstanceCount[0],
                                    nullptr);

  extensionInstanceCount[1] += extensionInstanceCount[0];

  const char ** pExtensionInstanceNames
      = new const char *[extensionInstanceCount[1]];

  bool found_extensions = SDL_Vulkan_GetInstanceExtensions (
      WINDOW, &extensionInstanceCount[0], pExtensionInstanceNames);

  // Desde aqui se deben agregar las otras extensiones, desde el conteo que
  // devuelve SDL en adelante
  pExtensionInstanceNames[extensionInstanceCount[0]] = "VK_KHR_get_surface_capabilities2";

  if (found_extensions)
	  std::future<void> ReturnVulkanList = std::async(std::launch::async, Patata::Log::VulkanList, pExtensionInstanceNames, extensionInstanceCount[1] - 1, "Instance Extensions");

  // Create Instance
  vk::InstanceCreateInfo InstanceInfo ({}, &PatataEngineInfo,
#if defined(DEBUG) && defined(PATATA_USE_VVL)
                                       1, &layer,
#else
                                       0, nullptr,
#endif
                                       extensionInstanceCount[1],
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
