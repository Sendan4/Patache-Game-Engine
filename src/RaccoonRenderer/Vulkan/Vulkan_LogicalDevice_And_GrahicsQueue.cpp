#include "Vulkan_LogicalDevice_And_GrahicsQueue.hpp"

uint32_t
Patata::Engine::CreateLogicalDeviceAndCreateQueue (void)
{
  uint32_t QueueCount                  = 0;
  float    QueuePriority               = 1.0f;
  bool     FoundGraphicsQueue          = false;
  uint32_t TMPGraphicsQueueFamilyIndex = 0;

  // find a queue of graphs
  Vulkan.PhysicalDevice.getQueueFamilyProperties (&QueueCount, nullptr);

  fast_io::io::println (
#if defined(_WIN64)
      fast_io::out (),
#endif
      PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA, "Raccoon Renderer",
      PATATA_TERM_RESET, PATATA_TERM_BOLD, " : Found ", QueueCount, " queues",
      PATATA_TERM_RESET);

  vk::QueueFamilyProperties * QueueFamilyProperties
      = new vk::QueueFamilyProperties[QueueCount];

  Vulkan.PhysicalDevice.getQueueFamilyProperties (&QueueCount,
                                                  QueueFamilyProperties);

  for (uint32_t i = 0; i < QueueCount; ++i)
    fast_io::io::println (
#if defined(_WIN64)
        fast_io::out (),
#endif
#if PATATA_DEBUG == 1
#if !defined(_WIN64)
        PATATA_TERM_DIM,
#endif
        PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "  [",
        std::string_view{ abi::__cxa_demangle (
            typeid (QueueFamilyProperties[i].queueFlags).name (), nullptr,
            nullptr, nullptr) },
        "] ",
#else
        "  [",
        std::string_view{
            typeid (QueueFamilyProperties[i].queueFlags).name () },
        "] ",
#endif
        PATATA_TERM_RESET,
#else  // PATATA_DEBUG
        "  ",
#endif // PATATA_DEBUG
        PATATA_TERM_BOLD, "Index [", PATATA_TERM_RESET, i, PATATA_TERM_BOLD,
        "] : ", PATATA_TERM_RESET,
        vk::to_string (QueueFamilyProperties[i].queueFlags));

  for (uint16_t index = 0; index < QueueCount; ++index)
    {
      if (QueueFamilyProperties[index].queueFlags
          & vk::QueueFlagBits::eGraphics)
        {
          TMPGraphicsQueueFamilyIndex = index;
          FoundGraphicsQueue          = true;

          fast_io::io::println (
#if defined(_WIN64)
              fast_io::out (),
#endif
              PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA, "Raccoon Renderer",
              PATATA_TERM_RESET, PATATA_TERM_BOLD, " : Found index ",
              TMPGraphicsQueueFamilyIndex, " that contains a graphics queue",
              PATATA_TERM_RESET);

          break;
        }
    }

  if (!FoundGraphicsQueue)
    {
      std::future<void> ReturnVulkanErr
          = std::async (std::launch::async, Patata::Log::FatalErrorMessage,
                        "Patata Engine - Raccoon Renderer",
                        "No Queue found for graphics", configuration);

      // returning the maximum of uint32_t is the equivalent of failing to find
      // the graph queue.
      return UINT32_MAX;
    }

#if PATATA_DEBUG == 1
  engineInfo.VkQueueIndex    = TMPGraphicsQueueFamilyIndex;
  engineInfo.VkQueuePriority = QueuePriority;
  engineInfo.VkQueueFlags
      = QueueFamilyProperties[TMPGraphicsQueueFamilyIndex].queueFlags;
#endif

  delete[] QueueFamilyProperties;
  QueueFamilyProperties = nullptr;

  vk::DeviceQueueCreateInfo DeviceQueueCreateInfo (
      {},
      TMPGraphicsQueueFamilyIndex, // Queue Family Index
      1,                           // Queue Count
      &QueuePriority,              // Queue Priority
      nullptr                      // pNext
  );

  const char * DeviceExtensions[1]{ "VK_KHR_swapchain" };

#if PATATA_DEBUG == 1
  engineInfo.ppVkDeviceExtensions    = new const char *[1];
  engineInfo.ppVkDeviceExtensions[0] = DeviceExtensions[0];
  engineInfo.VkDeviceExtensionsCount = 1;
#endif

  // Logical Device Info
  vk::DeviceCreateInfo DeviceCreateInfo (
      {},
      1,                      // Queue Create Info Count
      &DeviceQueueCreateInfo, // p Queue Create Infos
      0,                      // Enabled Layer Count
      nullptr,                // pp Enabled Layer Names
      1,                      // Enabled Extension Count
      DeviceExtensions,       // pp Enabled Extension Names
      nullptr,                // p Enabled Features
      nullptr                 // pNext
  );

  // List Device Extensions
  {
    fast_io::io::println (
#if defined(_WIN64)
        fast_io::out ()
#else
        ""
#endif
    );

    std::future<void> ReturnVulkanList
        = std::async (std::launch::async, Patata::Log::VulkanList,
                      DeviceExtensions, 1, "Device Extensions");
  }

  vk::Result Result = Vulkan.PhysicalDevice.createDevice (
      &DeviceCreateInfo, nullptr, &Vulkan.Device);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Logical Device", Result);

      std::future<void> ReturnVulkanErr
          = std::async (std::launch::async, Patata::Log::FatalErrorMessage,
                        "Patata Engine - Raccoon Renderer",
                        "Logical device creation failed", configuration);

      // returning the maximum of uint32_t is the equivalent of failing to find
      // the graph queue.
      return UINT32_MAX;
    }

  // Graphic queue associated with the logic device
  Vulkan.Queue = Vulkan.Device.getQueue (TMPGraphicsQueueFamilyIndex, 0);

  return TMPGraphicsQueueFamilyIndex;
}
