/*
The GPU with the highest score wins to be chosen later as the logic device
and also the physical device is used to obtain information about its
characteristics.

Discreet video cards are preferred.
Geometry shading support is obligatory.
The larger the maximum image size the GPU can handle, the better.
*/

#include "Vulkan_Select_Device.hpp"

bool
Patata::Engine::SelectDevice (void)
{
  uint32_t GpuCount = 0;

  // This function returns an extra number (Human Count from 1).
  vk::Result Result
      = Vulkan.Instance.enumeratePhysicalDevices (&GpuCount, nullptr);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck,
          "Enumerate Physical Devices - Obtaining the count", Result);

      return false;
    }

  if (GpuCount <= 0)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::FatalErrorMessage,
                        "Patata - Raccoon Renderer",
                        "No vulkan compatible device found", configuration);

      return false;
    }

  vk::PhysicalDevice * TmpPhysicalDevices;

  // If you only have one compatible GPU, I will take it directly.
  if (GpuCount == 1)
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA, "Raccoon Renderer",
          PATATA_TERM_RESET, PATATA_TERM_BOLD,
          " : Only one vulkan compatible device found", PATATA_TERM_RESET);

      TmpPhysicalDevices = new vk::PhysicalDevice[GpuCount];
      Result             = Vulkan.Instance.enumeratePhysicalDevices (&GpuCount,
                                                                     TmpPhysicalDevices);
      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patata::Log::VulkanCheck,
                            "Enumerate Physical Devices", Result);

          std::future<void> Err = std::async (
              std::launch::async, Patata::Log::FatalErrorMessage,
              "Patata Engine - RaccoonRenderer",
              "Failed to obtain a physical device", configuration);

          delete[] TmpPhysicalDevices;
          TmpPhysicalDevices = nullptr;
          return false;
        }

      Vulkan.PhysicalDevice = TmpPhysicalDevices[0];
      delete[] TmpPhysicalDevices;
      TmpPhysicalDevices = nullptr;

      vk::PhysicalDeviceFeatures DeviceFeatures
          = Vulkan.PhysicalDevice.getFeatures ();

      // I need the GPU to have geometryShader
      if (!DeviceFeatures.geometryShader)
        {
          Patata::Log::ErrorMessage ("Your GPU Dont Support Geometry Shader");
          Vulkan.PhysicalDevice = nullptr;
          return false;
        }

      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          "");

      return true;
    }
  else
    {
      TmpPhysicalDevices = new vk::PhysicalDevice[GpuCount];
      Result             = Vulkan.Instance.enumeratePhysicalDevices (&GpuCount,
                                                                     TmpPhysicalDevices);

      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patata::Log::VulkanCheck,
                            "Enumerate Physical Devices", Result);

          return false;
        }
    }

  fast_io::io::println (
#if defined(_WIN64)
      fast_io::out (),
#endif
      PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA, "Raccoon Renderer",
      PATATA_TERM_RESET, PATATA_TERM_BOLD, " : Found ", GpuCount,
      " vulkan compatible devices", PATATA_TERM_RESET);

  uint64_t * GpuScore = new uint64_t[GpuCount];

  vk::PhysicalDeviceProperties2 DeviceProperties;
  vk::PhysicalDeviceFeatures2   DeviceFeatures;

  for (uint32_t i = 0; i < GpuCount; ++i)
    {
      DeviceProperties = TmpPhysicalDevices[i].getProperties2 ();
      DeviceFeatures   = TmpPhysicalDevices[i].getFeatures2 ();

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

          std::future<void> Err = std::async (
              std::launch::async, Patata::Log::ErrorMessage,
              "This GPU Dont Support Geometry Shader, your points will be 0");
        }

      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#else
          PATATA_TERM_DIM,
#endif
          PATATA_TERM_COLOR_GRAY0, "  [",
          std::string_view{
              vk::to_string (DeviceProperties.properties.deviceType) },
          "] ", PATATA_TERM_RESET,
          std::string_view{ DeviceProperties.properties.deviceName }, " ",
          GpuScore[i]);
    }

  fast_io::io::println (
#if defined(_WIN64)
      fast_io::out ()
#else
      ""
#endif
  );

  uint64_t HighestScore   = 0;
  uint32_t GpuIndexWinner = 0;

  for (uint32_t i = 0; i < GpuCount; ++i)
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
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::FatalErrorMessage,
                        "Patata - Raccoon Renderer",
                        "Cannot select a device (GPU) which does not support "
                        "geometry shader",
                        configuration);

      return false;
    }

  // Select device
  Vulkan.PhysicalDevice = TmpPhysicalDevices[GpuIndexWinner];

  delete[] TmpPhysicalDevices;
  TmpPhysicalDevices = nullptr;
  delete[] GpuScore;

  return true;
}
