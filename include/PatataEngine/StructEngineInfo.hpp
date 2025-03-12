#if PATATA_DEBUG == 1
#pragma once

#define PATATA_VK_LAYER_COUNT             1
#define PATATA_VK_VERSION_SIZE            20
#define PATATA_VK_DRIVER_VERSION_SIZE     100
#define PATATA_VK_DRIVER_ID_SIZE          256
#define PATATA_VK_DEVICE_TYPE_SIZE        32
#define PATATA_VK_QUEUE_FAMILY_PROPERTIES 512

namespace Patata
{
struct EngineInfo
{
  // Window
  std::uint32_t WindowCreationFlags = 0;

  // RaccoonRenderer
  // Vulkan
  char VkVersion[PATATA_VK_VERSION_SIZE]{ 0 };

  // Device
  char          VkDeviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{ 0 };
  std::uint32_t VkDeviceVendorId = 0;
  char          VkDeviceType[PATATA_VK_DEVICE_TYPE_SIZE]{ 0 };

  // Queue
  // uint8_t VkQueuesCount = 0
  std::uint32_t                VkQueueIndex    = 0;
  float                        VkQueuePriority = 0.0f;
  vk::Flags<vk::QueueFlagBits> VkQueueFlags;

  // Driver
  char VkDriverName[VK_MAX_DRIVER_NAME_SIZE]{ 0 };
  char VkDriverId[PATATA_VK_DRIVER_ID_SIZE]{ 0 };
  char VkDriverInfo[VK_MAX_DRIVER_INFO_SIZE]{ 0 };
  char VkDriverVersion[PATATA_VK_DRIVER_VERSION_SIZE]{ 0 };

  // Swapchain
  vk::PresentModeKHR VkSwapchainPresentMode;
  vk::Format         VkSwapchainImageColorFormat;
  vk::ColorSpaceKHR  VkSwapchainImageColorSpace;
  std::uint32_t       VkMinImageCount = 0;

  // Extensions
  const char ** ppVkLayers                = nullptr;
  const char ** ppVkInstanceExtensions    = nullptr;
  std::uint32_t VkInstanceExtensionsCount = 0;
  const char ** ppVkDeviceExtensions      = nullptr;
  std::uint32_t VkDeviceExtensionsCount   = 0;

  // RaccoonSound

  // ImGui
  bool ShowMainMenuBar                 = true;
  bool PatataInfoWindow                = false;
  bool PatataConfigWindow              = false;
  bool PatataRaccoonRendererInfoWindow = false;
  bool ConfigBooleansTmp               = false;
};
}
#endif
