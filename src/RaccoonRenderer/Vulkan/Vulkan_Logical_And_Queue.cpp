#include "Vulkan_Logical_And_Queue.hpp"

uint32_t
Patata::Graphics::RaccoonRenderer::VulkanBackend::
    CreateLogicalDeviceAndCreateQueue (YAML::Node & Config)
{
  uint32_t QueueCount = 0;
  uint32_t                               GraphicsQueueFamilyIndex = 0;
  float                                  QueuePriority            = 1.0f;
  vk::Result Result;
  bool FoundGraphicsQueue = false;

  PhysicalDevice.getQueueFamilyProperties (&QueueCount, nullptr);

  fast_io::io::println (
#if defined(_WIN64)
    fast_io::out (),
#endif
#if !defined(_WIN64)
    PATATA_TERM_BOLD,
#endif
    PATATA_TERM_COLOR_PATATA, "Raccoon Renderer", PATATA_TERM_RESET,
    " : Found ", QueueCount, " Queues");

  vk::QueueFamilyProperties * QueueFamilyProperties = new vk::QueueFamilyProperties[QueueCount];

  PhysicalDevice.getQueueFamilyProperties (&QueueCount, QueueFamilyProperties);

  for (uint32_t i = 0; i < QueueCount; ++i)
	fast_io::io::println(
#if defined(_WIN64)
      fast_io::out (),
#endif
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
	  vk::to_string(QueueFamilyProperties[i].queueFlags));

  for (uint32_t indexFlag = 0; indexFlag < QueueCount; ++indexFlag) {
    if (QueueFamilyProperties[indexFlag].queueFlags & vk::QueueFlagBits::eGraphics)
      {
        GraphicsQueueFamilyIndex = indexFlag;
		FoundGraphicsQueue = true;

	  	fast_io::io::println (
#if defined(_WIN64)
    		fast_io::out (),
#endif
#if !defined(_WIN64)
    		PATATA_TERM_BOLD,
#endif
    		PATATA_TERM_COLOR_PATATA, "Raccoon Renderer", PATATA_TERM_RESET,
    		" : Found graphics queue");

        break;
      }
  }

  if (!FoundGraphicsQueue) {
	  Patata::Log::FatalErrorMessage("Patata - Raccoon Renderer", "No Queue found for graphics", Config);
	  return false;
  }

  delete[] QueueFamilyProperties;

  fast_io::io::println(
#if defined(_WIN64)
    fast_io::out (),
#endif
	"");

  vk::DeviceQueueCreateInfo DeviceQueueCreateInfo{};
  DeviceQueueCreateInfo.flags 			 = vk::DeviceQueueCreateFlagBits ();
  DeviceQueueCreateInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;
  DeviceQueueCreateInfo.queueCount       = 1;
  DeviceQueueCreateInfo.pQueuePriorities = &QueuePriority;

  const char * DeviceExtensions[]
      = { "VK_KHR_swapchain" };

  vk::DeviceCreateInfo DeviceCreateInfo{};
  DeviceCreateInfo.enabledExtensionCount   = 1;
  DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions;
  DeviceCreateInfo.pNext                   = {};
  DeviceCreateInfo.queueCreateInfoCount    = 1;
  DeviceCreateInfo.pQueueCreateInfos       = &DeviceQueueCreateInfo;

  Patata::Log::VulkanList (DeviceExtensions, std::size (DeviceExtensions) - 1,
                           "Device Extensions");

  Result = PhysicalDevice.createDevice (&DeviceCreateInfo, nullptr, &Device);

  Patata::Log::VulkanCheck ("Logical Device", Result);

  Queue = Device.getQueue (GraphicsQueueFamilyIndex, 0);
  return GraphicsQueueFamilyIndex;
}
