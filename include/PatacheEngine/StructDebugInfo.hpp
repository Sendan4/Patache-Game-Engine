#pragma once

#include <imgui.h>

#define PATACHE_LAYER_VK_COUNT         1U
#define PATACHE_VERSION_VK_SIZE        20U
#define PATACHE_DRIVER_VERSION_VK_SIZE 100U
#define PATACHE_DRIVERID_VK_SIZE       256U
#define PATACHE_DEVICETYPE_VK_SIZE     32U
#define PATACHE_MEMORY_HOST_VK_SIZE    510U

namespace Patache
{
#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ || __DragonFly__ \
    || __MidnightBSD__
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
  std::uint64_t windowCreationFlags = 0U;

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ || __DragonFly__ \
    || __MidnightBSD__
  Patache::WindowDecorationType windowDecorationType = Patache::WindowDecorationType::eNone;
#endif

  // RaccoonRenderer
  // Vulkan
  char versionVK[PATACHE_VERSION_VK_SIZE]{ 0 };

  // Device
  char                          deviceNameVK[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{ 0 };
  std::uint32_t                 deviceVendorIdVK = 0;
  char                          deviceTypeVK[PATACHE_DEVICETYPE_VK_SIZE]{ 0 };
  vk::PhysicalDeviceProperties2 physicalDeviceProperties;

  // Queue
  // uint8_t VkQueuesCount = 0
  float                        queuePriorityVK = 0.0F;
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
  std::uint32_t      minImageCountVK = 0U; // Also the actual number of images

  // Extensions
  const char ** ppLayersVK                = nullptr;
  const char ** ppInstanceExtensionsVK    = nullptr;
  std::uint32_t instanceExtensionsCountVK = 0U;
  const char ** ppDeviceExtensionsVK      = nullptr;
  std::uint32_t deviceExtensionsCountVK   = 0U;

  // Memory
  // Total Heap memory
  double       vramSize     = 0.0F;
  const char * vramSizeUnit = "Byte";

  // GPU-Device heap
  float ** ppVramMemoryDeviceSize     = nullptr;
  char **  ppVramMemoryDeviceSizeUnit = nullptr;
  char **  ppVramMemoryDeviceHeap     = nullptr;
  char **  ppVramMemoryDeviceType     = nullptr;

  // CPU-GPU Host visible
  float        vramMemoryHostSize      = 0.0F;
  const char * pVramMemoryHostSizeUnit = "Bytes";
  char         vramMemoryHostHeap[PATACHE_MEMORY_HOST_VK_SIZE];
  char         vramMemoryHostType[PATACHE_MEMORY_HOST_VK_SIZE];

  // vma Pool
  const char * pVramPoolSizePerBlockUnit  = "Bytes";
  float        vramPoolSizePerBlock       = 0.0F;
  const char * pVramPoolSizeAllBlocksUnit = "Bytes";
  float        vramPoolSizeAllBlocks      = 0.0F;

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
