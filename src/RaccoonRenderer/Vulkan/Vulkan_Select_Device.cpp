/*
The GPU with the highest score wins to be chosen later as the logic device
and also the physical device is used to obtain information about its characteristics.

Discreet video cards are preferred.
Geometry shading support is obligatory.
The larger the maximum image size the GPU can handle, the better.
*/

#include "Vulkan_Select_Device.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::SelectDevice (void)
{
  uint32_t GpuCount = 0;

  // This function returns an extra number (Human Count from 1).
  vk::Result Result = Instance.enumeratePhysicalDevices (&GpuCount, nullptr);
  {
      std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Enumerate Physical Devices - Obtaining the count", Result);
  }

  if (GpuCount <= 0)
    {
      Patata::Log::FatalErrorMessage ("Patata - Raccoon Renderer",
                                      "No vulkan compatible device found",
                                      *pConfiguration);
      return false;
    }

  vk::PhysicalDevice * TMPPhysicalDevice;

  // If you only have one compatible GPU, I will take it directly.
  if (GpuCount == 1)
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_BOLD,
          PATATA_TERM_COLOR_PATATA, "\nRaccoon Renderer", PATATA_TERM_RESET,
          PATATA_TERM_BOLD,
          " : Only one vulkan compatible device found\n", PATATA_TERM_RESET);

      PhysicalDevice = Instance.enumeratePhysicalDevices ().front ();
      vk::PhysicalDeviceFeatures DeviceFeatures
          = PhysicalDevice.getFeatures ();

      // I need the GPU to have geometryShader
      if (!DeviceFeatures.geometryShader)
        {
          Patata::Log::ErrorMessage ("Your GPU Dont Support Geometry Shader");
          PhysicalDevice = nullptr;
          return false;
        }

      return true;
    }
  else
    {
      TMPPhysicalDevice = new vk::PhysicalDevice[GpuCount];
      Result
          = Instance.enumeratePhysicalDevices (&GpuCount, TMPPhysicalDevice);

	  std::future<void> ReturnVulkanCheck = std::async(std::launch::async, Patata::Log::VulkanCheck, "Enumerate Physical Devices", Result);
    }

  fast_io::io::println (
#if defined(_WIN64)
      fast_io::out (),
#endif
      PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA, "\nRaccoon Renderer", PATATA_TERM_RESET,
      PATATA_TERM_BOLD, " : Found ", GpuCount, " vulkan compatible devices", PATATA_TERM_RESET);

  uint64_t * GpuScore = new uint64_t[GpuCount];

  for (uint32_t i = 0; i < GpuCount; ++i)
    {
      vk::PhysicalDeviceProperties2 DeviceProperties
          = TMPPhysicalDevice[i].getProperties2 ();
      vk::PhysicalDeviceFeatures2 DeviceFeatures
          = TMPPhysicalDevice[i].getFeatures2 ();

      switch (DeviceProperties.properties.deviceType)
        {
        case vk::PhysicalDeviceType::eDiscreteGpu:
          GpuScore[i] += 1000;
          break;
        case vk::PhysicalDeviceType::eIntegratedGpu:
          GpuScore[i] += 50;
          break;

        case vk::PhysicalDeviceType::eCpu:
        case vk::PhysicalDeviceType::eOther:
        case vk::PhysicalDeviceType::eVirtualGpu:
          break;
        }

      // Maximum possible texture size.
      GpuScore[i] += DeviceProperties.properties.limits.maxImageDimension2D;

      // I need the GPU to have geometryShader
      if (!DeviceFeatures.features.geometryShader)
        {
          GpuScore[i] = 0;

          Patata::Log::ErrorMessage ("Your GPU Dont Support Geometry Shader");
        }

      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
#if !defined(_WIN64)
		  PATATA_TERM_DIM,
#endif
          PATATA_TERM_COLOR_GRAY0, "  [",
          std::string_view{ vk::to_string (DeviceProperties.properties.deviceType) },
          "] ", PATATA_TERM_RESET,
          std::string_view{ DeviceProperties.properties.deviceName }, " ", GpuScore[i]);
    }

  fast_io::io::println (
#if defined(_WIN64)
      fast_io::out (),
#endif
      "");

  uint64_t HighestScore   = 0;
  uint32_t GpuIndexWinner = 0;

  for (uint32_t i = 0; i < GpuCount; i++)
    {
      if (GpuScore[i] > HighestScore)
        {
          HighestScore   = GpuScore[i];
          GpuIndexWinner = i;
        }
    }

  // I am very heavy on geometryShader support.
  if (GpuScore[GpuIndexWinner] == 0)
    {
      Patata::Log::FatalErrorMessage ("Patata - Raccoon Renderer",
                                      "Cannot select a device (GPU) which does not support geometry shader",
                                      *pConfiguration);
      return false;
    }

  // Select device
  PhysicalDevice = TMPPhysicalDevice[GpuIndexWinner];

  delete[] TMPPhysicalDevice;
  delete[] GpuScore;

  return true;
}
