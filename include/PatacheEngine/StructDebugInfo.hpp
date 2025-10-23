#if PATACHE_DEBUG == 1
  #pragma once

  #include <imgui.h>

  #define PATACHE_LAYER_VK_COUNT         1
  #define PATACHE_VERSION_VK_SIZE        20
  #define PATACHE_DRIVER_VERSION_VK_SIZE 100
  #define PATACHE_DRIVERID_VK_SIZE       256
  #define PATACHE_DEVICETYPE_VK_SIZE     32

namespace Patache
{
  #if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__  \
      || __DragonFly__ || __MidnightBSD__
enum class WindowDecorationType : std::uint8_t
{
  eNone,
  eClientSideDecoration,
  eServerSideDecoration
};
  #endif

struct EngineInfo
{
  // Window
  std::uint64_t windowCreationFlags = 0;
  #if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__  \
      || __DragonFly__ || __MidnightBSD__
  Patache::WindowDecorationType windowDecorationType = Patache::WindowDecorationType::eNone;
  #endif

  // RaccoonRenderer
  // Vulkan
  char versionVK[PATACHE_VERSION_VK_SIZE]{ 0 };

  // Device
  char          deviceNameVK[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{ 0 };
  std::uint32_t deviceVendorIdVK = 0;
  char          deviceTypeVK[PATACHE_DEVICETYPE_VK_SIZE]{ 0 };

  // Queue
  // uint8_t VkQueuesCount = 0
  float                        queuePriorityVK = 0.0f;
  vk::Flags<vk::QueueFlagBits> queueFlagsVK;

  // Driver
  char driverNameVK[VK_MAX_DRIVER_NAME_SIZE]{ 0 };
  char driverIdVK[PATACHE_DRIVERID_VK_SIZE]{ 0 };
  char driverInfoVK[VK_MAX_DRIVER_INFO_SIZE]{ 0 };
  char driverVersionVK[PATACHE_DRIVER_VERSION_VK_SIZE]{ 0 };

  // Swapchain
  vk::PresentModeKHR swapchainPresentModeVK;
  vk::Format         swapchainImageColorFormatVK;
  vk::ColorSpaceKHR  swapchainImageColorSpaceVK;
  std::uint32_t      minImageCountVK = 0; // Also the actual number of images

  // Extensions
  const char ** ppLayersVK                = nullptr;
  const char ** ppInstanceExtensionsVK    = nullptr;
  std::uint32_t instanceExtensionsCountVK = 0;
  const char ** ppDeviceExtensionsVK      = nullptr;
  std::uint32_t deviceExtensionsCountVK   = 0;

  // Memory
  double       vramSizeVK     = 0;
  const char * vramSizeUnitVK = "Byte";

  // RaccoonSound

  // ImGui
  bool showMainMenuBar           = true;
  bool infoWindow                = false;
  bool configWindow              = false;
  bool raccoonRendererInfoWindow = false;
  bool configBooleansTmp         = false;

  ImVec4 engineStyles[21];
};
}
#endif
