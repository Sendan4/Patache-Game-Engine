#if PATATA_DEBUG == 1
#pragma once

#define PATATA_VK_LAYER_COUNT         1
#define PATATA_VK_VERSION_SIZE        20
#define PATATA_VK_DRIVER_VERSION_SIZE 100
#define PATATA_VK_DRIVER_ID_SIZE      256
#define PATATA_VK_DEVICE_TYPE_SIZE    32

namespace Patata
{
struct EngineInfo
{
// Window
#if defined(__linux__)
  const char * Desktop     = nullptr;
  const char * SessionType = nullptr;
#endif
  SDL_SysWMinfo WindowInfo;
  uint32_t      WindowCreationFlags = 0;

  // RaccoonRenderer
  // Vulkan
  char VkVersion[PATATA_VK_VERSION_SIZE]{ 0 };

  // Device
  char     VkDeviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{ 0 };
  uint32_t VkDeviceVendorId = 0;
  char     VkDeviceType[PATATA_VK_DEVICE_TYPE_SIZE]{ 0 };

  // Driver
  char VkDriverName[VK_MAX_DRIVER_NAME_SIZE]{ 0 };
  char VkDriverId[PATATA_VK_DRIVER_ID_SIZE]{ 0 };
  char VkDriverInfo[VK_MAX_DRIVER_INFO_SIZE]{ 0 };
  char VkDriverVersion[PATATA_VK_DRIVER_VERSION_SIZE]{ 0 };

  // Swapchain
  vk::PresentModeKHR VkSwapchainPresentMode;
  vk::Format         VkSwapchainImageColorFormat;
  vk::ColorSpaceKHR  VkSwapchainImageColorSpace;

  // Extensions
  const char ** ppVkLayers                = nullptr;
  const char ** ppVkInstanceExtensions    = nullptr;
  uint32_t      VkInstanceExtensionsCount = 0;
  const char ** ppVkDeviceExtensions      = nullptr;
  uint32_t      VkDeviceExtensionsCount   = 0;

  // RaccoonSound

  // ImGui
  bool ShowMainMenuBar    = true;
  bool PatataInfoWindow   = false;
  bool PatataConfigWindow = false;
};
}
#endif
