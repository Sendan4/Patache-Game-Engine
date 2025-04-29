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
Patache::Engine::SelectDevice (void)
{
  std::uint32_t GpuCount = 0;

  vk::Result Result
      = Vulkan.Instance.enumeratePhysicalDevices (&GpuCount, nullptr);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck,
          "Enumerate Physical Devices - Obtaining the count", Result);

      return false;
    }

  if (GpuCount <= 0)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache engine - Raccoon Renderer",
                        "No vulkan compatible device found", configuration);

      return false;
    }

  vk::PhysicalDevice * TmpPhysicalDevices;

  // If you only have one compatible GPU, I will take it directly.
  if (GpuCount == 1)
    {
      fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                            PATACHE_TERM_COLOR_PATACHE, "Raccoon Renderer",
                            PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                            " : Only one vulkan compatible device found",
                            PATACHE_TERM_RESET);

      TmpPhysicalDevices = new vk::PhysicalDevice[GpuCount];
      Result             = Vulkan.Instance.enumeratePhysicalDevices (&GpuCount,
                                                                     TmpPhysicalDevices);
      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patache::Log::VulkanCheck,
                            "Enumerate Physical Devices", Result);

          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::FatalErrorMessage,
              "Patache Engine - RaccoonRenderer",
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
          Patache::Log::ErrorMessage ("Your GPU Dont Support Geometry Shader");
          Vulkan.PhysicalDevice = nullptr;
          return false;
        }

      fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);

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
              = std::async (std::launch::async, Patache::Log::VulkanCheck,
                            "Enumerate Physical Devices", Result);

          return false;
        }
    }

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
      "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found ",
      GpuCount, " vulkan compatible devices", PATACHE_TERM_RESET);

  std::uint64_t * GpuScore = new uint64_t[GpuCount];

  vk::PhysicalDeviceProperties2 DeviceProperties;
  vk::PhysicalDeviceFeatures2   DeviceFeatures;

  for (std::uint32_t i = 0; i < GpuCount; ++i)
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
              std::launch::async, Patache::Log::ErrorMessage,
              "This GPU Dont Support Geometry Shader, your points will be 0");
        }

      fast_io::io::println (
          PATACHE_FAST_IO_BUFF_OUT,
#if !defined(_WIN64)
          PATACHE_TERM_DIM,
#endif
          PATACHE_TERM_COLOR_GRAY0, "  [",
          vk::to_string (DeviceProperties.properties.deviceType), "] ",
          PATACHE_TERM_RESET,
          fast_io::mnp::os_c_str (DeviceProperties.properties.deviceName), " ",
          GpuScore[i]);
    }

  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);

  std::uint64_t HighestScore   = 0;
  std::uint32_t GpuIndexWinner = 0;

  for (std::uint32_t i = 0; i < GpuCount; ++i)
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
          = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                        "Patache Engine - Raccoon Renderer",
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
