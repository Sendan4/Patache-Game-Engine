#include "Vulkan_Info.hpp"

void
VulkanInfo (Patache::Engine * const pEngine, const Patache::SwapchainInfo & rSwapchainInfo)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                        "Raccoon Renderer ", PATACHE_TERM_RESET, "INFO");

  VkPhysicalDeviceDriverProperties driver{ .sType
                                           = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
                                           .pNext = nullptr,
                                           .driverID{},
                                           .driverName{},
                                           .driverInfo{},
                                           .conformanceVersion{} };
  VkPhysicalDeviceProperties2      physicalDeviceProperties{
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = &driver, .properties{}
  };
  vkGetPhysicalDeviceProperties2 (pEngine->vulkan.physicalDevice, &physicalDeviceProperties);

  std::uint64_t vramSizeTmp{ 0U };
  double        vramSize{ 0.0F };
  const char *  vramSizeUnit{ "Byte" };

  VkPhysicalDeviceMemoryProperties memProperties{};
  vkGetPhysicalDeviceMemoryProperties (pEngine->vulkan.physicalDevice, &memProperties);
#if PATACHE_DEBUG == 1
  vkGetPhysicalDeviceMemoryProperties (pEngine->vulkan.physicalDevice,
                                       &pEngine->debugInfo.memProperties);
#endif

  for (std::uint32_t i = 0; i < memProperties.memoryHeapCount; ++i)
    {
      if (memProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
          vramSizeTmp += static_cast<std::uint64_t> (memProperties.memoryHeaps[i].size);
        }
    }

#if PATACHE_DEBUG == 1
  /*
   * Storing information about pEngine->vulkan to display it through ImGui.
   * Only on debug builds
   */

  // Vulkan Version
  std::snprintf (pEngine->debugInfo.versionVK, PATACHE_VERSION_VK_SIZE, "%u.%u.%u.%u",
                 VK_VERSION_MAJOR (physicalDeviceProperties.properties.apiVersion),
                 VK_VERSION_MINOR (physicalDeviceProperties.properties.apiVersion),
                 VK_VERSION_PATCH (physicalDeviceProperties.properties.apiVersion),
                 VK_API_VERSION_VARIANT (physicalDeviceProperties.properties.apiVersion));

  // Vulkan Device Name
  PATACHE_STRNCPY (pEngine->debugInfo.deviceNameVK, physicalDeviceProperties.properties.deviceName,
                   VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, VK_MAX_EXTENSION_NAME_SIZE);

  // Vulkan Device Vendor ID
  pEngine->debugInfo.deviceVendorIdVK
      = static_cast<uint32_t> (physicalDeviceProperties.properties.vendorID);

  // Vulkan Device Type
  PATACHE_STRNCPY (pEngine->debugInfo.deviceTypeVK,
                   string_VkPhysicalDeviceType (physicalDeviceProperties.properties.deviceType),
                   PATACHE_DEVICETYPE_VK_SIZE, PATACHE_DEVICETYPE_VK_SIZE);

  // Vulkan Driver Name
  PATACHE_STRNCPY (pEngine->debugInfo.driverNameVK, driver.driverName, VK_MAX_DRIVER_NAME_SIZE,
                   VK_MAX_EXTENSION_NAME_SIZE);

  // Vulkan Driver ID
  PATACHE_STRNCPY (pEngine->debugInfo.driverIdVK, string_VkDriverId (driver.driverID),
                   PATACHE_DRIVERID_VK_SIZE, PATACHE_DRIVERID_VK_SIZE);

  // Vulkan Driver Info
  PATACHE_STRNCPY (pEngine->debugInfo.driverInfoVK, driver.driverInfo, VK_MAX_DRIVER_INFO_SIZE,
                   VK_MAX_EXTENSION_NAME_SIZE);

  // Vulkan Driver Version
  std::snprintf (pEngine->debugInfo.driverVersionVK, PATACHE_DRIVER_VERSION_VK_SIZE, "%u.%u.%u.%u",
                 VK_VERSION_MAJOR (physicalDeviceProperties.properties.driverVersion),
                 VK_VERSION_MINOR (physicalDeviceProperties.properties.driverVersion),
                 VK_VERSION_PATCH (physicalDeviceProperties.properties.driverVersion),
                 VK_API_VERSION_VARIANT (physicalDeviceProperties.properties.driverVersion));
#endif

  // VRAM HEAP size
  if (vramSizeTmp >= 1073741824U)
    {
#if PATACHE_DEBUG == 1
      pEngine->debugInfo.vramSizeUnit = "GiB";
      pEngine->debugInfo.vramSize     = vramSizeTmp / 1073741824.0F;
#endif
      vramSizeUnit = "GiB";
      vramSize     = vramSizeTmp / 1073741824.0F;

      goto END_CHECK_SIZE_UNIT;
    }

  if (vramSizeTmp >= 1048576U)
    {
#if PATACHE_DEBUG == 1
      pEngine->debugInfo.vramSizeUnit = "MiB";
      pEngine->debugInfo.vramSize     = vramSizeTmp / 1048576.0F;
#endif
      vramSizeUnit = "MiB";
      vramSize     = vramSizeTmp / 1048576.0F;

      goto END_CHECK_SIZE_UNIT;
    }

  if (vramSizeTmp >= 1024U)
    {
#if PATACHE_DEBUG == 1
      pEngine->debugInfo.vramSizeUnit = "KiB";
      pEngine->debugInfo.vramSize     = vramSizeTmp / 1024.0F;
#endif
      vramSizeUnit = "KiB";
      vramSize     = vramSizeTmp / 1024.0F;
    }

END_CHECK_SIZE_UNIT:

  // =================== Vulkan Version ===========================
  fast_io::io::println (
      PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, fast_io::mnp::right ("Vulkan Version : ", 19U),
      PATACHE_TERM_RESET, VK_VERSION_MAJOR (physicalDeviceProperties.properties.apiVersion), ".",
      VK_VERSION_MINOR (physicalDeviceProperties.properties.apiVersion), ".",
      VK_VERSION_PATCH (physicalDeviceProperties.properties.apiVersion), ".",
      VK_API_VERSION_VARIANT (physicalDeviceProperties.properties.apiVersion), " | ",
      PATACHE_TERM_BOLD, "In Use ", PATACHE_TERM_RESET, VK_VERSION_MAJOR (VK_API_VERSION_1_2), ".",
      VK_VERSION_MINOR (VK_API_VERSION_1_2), "\n");

  // =================== Device ===================================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Device", 8U), PATACHE_TERM_RESET);

  // =================== Device Name ==============================
  {
    fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Name : ", 11U), PATACHE_TERM_RESET);

    switch (physicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_GREEN);
        break;
      }

    fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
                          fast_io::mnp::os_c_str (physicalDeviceProperties.properties.deviceName),
                          PATACHE_TERM_RESET);
  }

  // =================== Device Vendor ============================
  {
    const char * pVendor = nullptr;

    switch (physicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        pVendor = "Intel";
        break;

      case 4098:
        pVendor = "AMD";
        break;

      case 4318:
        pVendor = "Nvidia";
        break;

      case 65541:
        pVendor = "Mesa";
        break;

      default:
        pVendor = "Unknown";
        break;
      }

    fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Vendor : ", 13U), PATACHE_TERM_RESET);

    // String Vendor
    switch (physicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_BLUE,
                            fast_io::mnp::os_c_str (pVendor), PATACHE_TERM_RESET);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_RED,
                            fast_io::mnp::os_c_str (pVendor), PATACHE_TERM_RESET);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_GREEN,
                            fast_io::mnp::os_c_str (pVendor), PATACHE_TERM_RESET);
        break;

      case 65541:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_GRAY1,
                            fast_io::mnp::os_c_str (pVendor), PATACHE_TERM_RESET);
        break;

      default:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::os_c_str (pVendor));
        break;
      }

    // Hexadecimal VendorID
    fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0,
                        " | [Hexadecimal] ", PATACHE_TERM_RESET);

    switch (physicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_GREEN);
        break;
      }

    fast_io::io::print (PATACHE_FASTIO_BUFFOUT,
                        fast_io::mnp::hex (physicalDeviceProperties.properties.vendorID),
                        PATACHE_TERM_RESET, " ");

    // Decimal VendorID
    fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0,
                        "| [Decimal] ", PATACHE_TERM_RESET);

    switch (physicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT, physicalDeviceProperties.properties.vendorID,
                          PATACHE_TERM_RESET);
  }

  // =================== Device Type ==============================
  // Device Type
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Type : ", 11U), PATACHE_TERM_RESET,
                        fast_io::mnp::os_c_str (string_VkPhysicalDeviceType (
                            physicalDeviceProperties.properties.deviceType)));

  // =================== Device Vram Size =========================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Vram Size : ", 16U), PATACHE_TERM_RESET, vramSize,
                        " ", fast_io::mnp::os_c_str (vramSizeUnit), "\n");

  // =================== Driver ===================================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Driver", 8U), PATACHE_TERM_RESET);

  // =================== Driver Name ==============================

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Name : ", 11U), PATACHE_TERM_RESET,
                        fast_io::mnp::os_c_str (driver.driverName));

  // =================== Driver ID ================================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("ID : ", 9U), PATACHE_TERM_RESET,
                        fast_io::mnp::os_c_str (string_VkDriverId (driver.driverID)));

  // =================== Driver Info ==============================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Info : ", 11U), PATACHE_TERM_RESET,
                        fast_io::mnp::os_c_str (driver.driverInfo));

  // =================== Driver Version ===========================
  fast_io::io::println (
      PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, fast_io::mnp::right ("Version : ", 14U),
      PATACHE_TERM_RESET, VK_VERSION_MAJOR (physicalDeviceProperties.properties.driverVersion), ".",
      VK_VERSION_MINOR (physicalDeviceProperties.properties.driverVersion), ".",
      VK_VERSION_PATCH (physicalDeviceProperties.properties.driverVersion), ".",
      VK_API_VERSION_VARIANT (physicalDeviceProperties.properties.driverVersion), "\n");

  // =================== Swapchain ================================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Swapchain", 11U), PATACHE_TERM_RESET);

  // =================== Swapchain Present Mode ===================
  fast_io::io::print (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                      fast_io::mnp::right ("Present Mode : ", 19U), PATACHE_TERM_RESET,
                      fast_io::mnp::os_c_str (string_VkPresentModeKHR (rSwapchainInfo.presentMode)),
                      " ");

  // Vulkan Vsync
  if ((rSwapchainInfo.presentMode == VK_PRESENT_MODE_FIFO_KHR
       || rSwapchainInfo.presentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
      && pEngine->configuration.vsync)
    {
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_GREEN, "Vertical Sync",
                            PATACHE_TERM_RESET);
    }
  else
    {
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT);
    }

  // =================== Swapchain Images =========================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Image Count : ", 18U), PATACHE_TERM_RESET,
                        pEngine->vulkan.swapchainImageCount);

  // =================== Swapchain Image Color Format =============
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Surface Color Format : ", 27U), PATACHE_TERM_RESET,
                        fast_io::mnp::os_c_str (string_VkFormat (rSwapchainInfo.imageColorFormat)));

  // =================== Swapchain Image Color Format =============
  fast_io::io::println (
      PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
      fast_io::mnp::right ("Surface Color Space : ", 26U), PATACHE_TERM_RESET,
      fast_io::mnp::os_c_str (string_VkColorSpaceKHR (rSwapchainInfo.imageColorSpace)), "\n");

  // =================== Debug ====================================
#if PATACHE_DEBUG == 1
  #if defined(PATACHE_IMGUI_VERSION)
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("ImGui Version : ", 18U), PATACHE_TERM_RESET,
                        PATACHE_IMGUI_VERSION);
  #endif
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT);
}
