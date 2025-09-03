#if PATACHE_DEBUG == 1
#pragma once

#include <imgui.h>

#define PATACHE_VK_LAYER_COUNT             1
#define PATACHE_VK_VERSION_SIZE            20
#define PATACHE_VK_DRIVER_VERSION_SIZE     100
#define PATACHE_VK_DRIVER_ID_SIZE          256
#define PATACHE_VK_DEVICE_TYPE_SIZE        32
#define PATACHE_VK_QUEUE_FAMILY_PROPERTIES 512

namespace Patache
{
#if defined(__linux__)
enum class WindowDecorationType : std::uint8_t
{
  None,
  ClientSideDecoration,
  ServerSideDecoration
};
#endif

struct EngineInfo
{
  // Window
  std::uint64_t WindowCreationFlags = 0;
#if defined(__linux__)
  Patache::WindowDecorationType windowDecorationType
      = Patache::WindowDecorationType::None;
#endif

  // RaccoonRenderer
  // Vulkan
  char VkVersion[PATACHE_VK_VERSION_SIZE]{ 0 };

  // Device
  char          VkDeviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{ 0 };
  std::uint32_t VkDeviceVendorId = 0;
  char          VkDeviceType[PATACHE_VK_DEVICE_TYPE_SIZE]{ 0 };

  // Queue
  // uint8_t VkQueuesCount = 0
  float                        VkQueuePriority = 0.0f;
  vk::Flags<vk::QueueFlagBits> VkQueueFlags;

  // Driver
  char VkDriverName[VK_MAX_DRIVER_NAME_SIZE]{ 0 };
  char VkDriverId[PATACHE_VK_DRIVER_ID_SIZE]{ 0 };
  char VkDriverInfo[VK_MAX_DRIVER_INFO_SIZE]{ 0 };
  char VkDriverVersion[PATACHE_VK_DRIVER_VERSION_SIZE]{ 0 };

  // Swapchain
  vk::PresentModeKHR VkSwapchainPresentMode;
  vk::Format         VkSwapchainImageColorFormat;
  vk::ColorSpaceKHR  VkSwapchainImageColorSpace;
  std::uint32_t      VkMinImageCount = 0; // Also the actual number of images

  // Extensions
  const char ** ppVkLayers                = nullptr;
  const char ** ppVkInstanceExtensions    = nullptr;
  std::uint32_t VkInstanceExtensionsCount = 0;
  const char ** ppVkDeviceExtensions      = nullptr;
  std::uint32_t VkDeviceExtensionsCount   = 0;

  // Memory
  double       VkVramSize     = 0;
  const char * VkVramSizeUnit = "Byte";

  // RaccoonSound

  // ImGui
  bool ShowMainMenuBar                  = true;
  bool PatacheInfoWindow                = false;
  bool PatacheConfigWindow              = false;
  bool PatacheRaccoonRendererInfoWindow = false;
  bool ConfigBooleansTmp                = false;

  ImVec4 engineStyles[21];
};
}
#endif
