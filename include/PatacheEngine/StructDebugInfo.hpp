#pragma once

#include <chrono>
#include "PatacheEngine/Queue.hpp"

#include <imgui.h>

#define PATACHE_LAYER_VK_COUNT                   1U
#define PATACHE_VERSION_VK_SIZE_EXTRANULL        20U
#define PATACHE_VERSION_VK_SIZE                  19U
#define PATACHE_DRIVER_VERSION_VK_SIZE_EXTRANULL 100U
#define PATACHE_DRIVER_VERSION_VK_SIZE           99U
#define PATACHE_DRIVERID_VK_SIZE_EXTRANULL       256U
#define PATACHE_DRIVERID_VK_SIZE                 255U
#define PATACHE_DEVICETYPE_VK_SIZE_EXTRANULL     64U
#define PATACHE_DEVICETYPE_VK_SIZE               63U
#define PATACHE_MEMORY_HOST_VK_SIZE_EXTRANULL    510U
#define PATACHE_MEMORY_HOST_VK_SIZE              509U
#define PATACHE_MEMINFO_STR_SIZE_EXTRANULL       64U
#define PATACHE_MEMINFO_STR_SIZE                 63U
#define PATACHE_SIZEUNIT_STR_SIZE_EXTRANULL      8U
#define PATACHE_SIZEUNIT_STR_SIZE                7U
#define PATACHE_FPSHISTORY_COUNT                 64U

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
  float                                                       startTime{ 0U };
  std::chrono::time_point<std::chrono::high_resolution_clock> startTimer{};

  // Window
  std::uint64_t windowCreationFlags{ 0U };

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ || __DragonFly__ \
    || __MidnightBSD__
  Patache::WindowDecorationType windowDecorationType{ Patache::WindowDecorationType::eNone };
#endif

  // RaccoonRenderer
  // Vulkan
  char versionVK[PATACHE_VERSION_VK_SIZE_EXTRANULL]{ 0 };

  // Device
  char                        deviceNameVK[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{};
  std::uint32_t               deviceVendorIdVK{ 0U };
  char                        deviceTypeVK[PATACHE_DEVICETYPE_VK_SIZE_EXTRANULL]{};
  VkPhysicalDeviceProperties2 physicalDeviceProperties{
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = nullptr, .properties{}
  };

  // Queue
  float        queuePriorityVK{ 0.0F };
  VkQueueFlags queueFlagsVK;

  // Driver
  char driverNameVK[VK_MAX_DRIVER_NAME_SIZE]{};
  char driverIdVK[PATACHE_DRIVERID_VK_SIZE_EXTRANULL]{};
  char driverInfoVK[VK_MAX_DRIVER_INFO_SIZE]{};
  char driverVersionVK[PATACHE_DRIVER_VERSION_VK_SIZE_EXTRANULL]{};

  // Swapchain
  VkPresentModeKHR swapchainPresentModeVK;
  VkFormat         swapchainImageColorFormatVK;
  VkColorSpaceKHR  swapchainImageColorSpaceVK;
  std::uint32_t    minImageCountVK{ 0U }; // Also the actual number of images

  // Extensions
  const char ** ppLayersVK{ nullptr };
  const char ** ppInstanceExtensionsVK{ nullptr };
  std::uint32_t instanceExtensionsCountVK{ 0U };
  const char ** ppDeviceExtensionsVK{ nullptr };
  std::uint32_t deviceExtensionsCountVK{ 0U };

  // Memory
  // Total Heap memory
  double       vramSize{ 0.0F };
  const char * vramSizeUnit{ "Byte" };

  // GPU-Device heap
  float ** ppVramMemoryDeviceSize{ nullptr };
  char **  ppVramMemoryDeviceSizeUnit{ nullptr };
  char **  ppVramMemoryDeviceHeap{ nullptr };
  char **  ppVramMemoryDeviceType{ nullptr };

  // CPU-GPU Host visible
  float        vramMemoryHostSize{ 0.0F };
  const char * pVramMemoryHostSizeUnit{ "Bytes" };
  char         vramMemoryHostHeap[PATACHE_MEMORY_HOST_VK_SIZE_EXTRANULL]{};
  char         vramMemoryHostType[PATACHE_MEMORY_HOST_VK_SIZE_EXTRANULL]{};

  // VMA & Memory
  char                             pSizeUnit[PATACHE_SIZEUNIT_STR_SIZE_EXTRANULL]{ "Bytes" };
  char                             pSizeUnit2[PATACHE_SIZEUNIT_STR_SIZE_EXTRANULL]{ "Bytes" };
  VmaBudget                        memBudgetInfo[VK_MAX_MEMORY_HEAPS]{};
  VmaTotalStatistics               memStats{};
  VkPhysicalDeviceMemoryProperties memProperties{};
  float                            memSize{ 0.0F };
  float                            memSize2{ 0.0F };
  std::uint64_t                    memSizeBytes{ 0U };
  std::uint64_t                    memSizeBytes2{ 0U };
  char                             memInfo[PATACHE_MEMINFO_STR_SIZE_EXTRANULL]{};
  float                            barProgress{ 0.0F };

  // debug fps counter
  std::uint64_t numFrames{ 0U };
  float         fps{ 0.0F };
  float         fpsHistory[PATACHE_FPSHISTORY_COUNT]{ 0.0F };
  std::uint16_t fpsHistoryIterator{ 0U };
  float         frameTimeHistory[PATACHE_FPSHISTORY_COUNT]{ 0.0F };
  std::uint16_t frameTimeHistoryIterator{ 0U };
  float         frameTime{ 0.0F };
  float         mainCmdTime{ 0U };
  float         mainRenderPassTime{ 0U };
  float         mainSwapchainTime{ 0U };
  bool          timerFpsRun{ true };
  bool          calculatePerformanceStats{ false };
  std::chrono::time_point<std::chrono::high_resolution_clock> frameTimer{};
  std::chrono::time_point<std::chrono::high_resolution_clock> mainRenderPassTimer{};
  std::chrono::time_point<std::chrono::high_resolution_clock> mainCmdTimer{};
  std::chrono::time_point<std::chrono::high_resolution_clock> mainSwapchainTimer{};

  // RaccoonSound

  // ImGui
  bool showMainMenuBar{ true };
  bool infoWindow{ false };
  bool configWindow{ false };
  bool raccoonRendererInfoWindow{ false };
  bool configBooleansTmp{ false };

  ImVec4 engineStyles[21];
};
}
