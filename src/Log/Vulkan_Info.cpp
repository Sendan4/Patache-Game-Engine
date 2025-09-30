#include "Vulkan_Info.hpp"

void
VulkanInfo (Patache::Engine * const pEngine, const Patache::SwapchainInfo & rSwapchainInfo)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                        "Raccoon Renderer ", PATACHE_TERM_RESET, "INFO");

  vk::PhysicalDeviceDriverProperties driver{};

  vk::PhysicalDeviceProperties2 physicalDeviceProperties{};
  physicalDeviceProperties.pNext = &driver;
  pEngine->vulkan.physicalDevice.getProperties2 (&physicalDeviceProperties);

  std::uint64_t vramSizeTmp  = 0;
  double        vramSize     = 0;
  const char *  vramSizeUnit = "Byte";

  vk::PhysicalDeviceMemoryProperties memProperties{};
  pEngine->vulkan.physicalDevice.getMemoryProperties (&memProperties);

  for (std::uint32_t i = 0; i < memProperties.memoryHeapCount; ++i)
    {
      if (memProperties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
        vramSizeTmp += static_cast<std::uint64_t> (memProperties.memoryHeaps[i].size);
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
  PATACHE_STRNCPY (pEngine->debugInfo.deviceNameVK,
                   physicalDeviceProperties.properties.deviceName.data (),
                   VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);

  // Vulkan Device Vendor ID
  pEngine->debugInfo.deviceVendorIdVK
      = static_cast<uint32_t> (physicalDeviceProperties.properties.vendorID);

  // Vulkan Device Type
  PATACHE_STRNCPY (pEngine->debugInfo.deviceTypeVK,
                   vk::to_string (physicalDeviceProperties.properties.deviceType).c_str (),
                   PATACHE_DEVICETYPE_VK_SIZE);

  // Vulkan Driver Name
  PATACHE_STRNCPY (pEngine->debugInfo.driverNameVK, driver.driverName.data (),
                   VK_MAX_DRIVER_NAME_SIZE);

  // Vulkan Driver ID
  PATACHE_STRNCPY (pEngine->debugInfo.driverIdVK, vk::to_string (driver.driverID).c_str (),
                   PATACHE_DRIVERID_VK_SIZE);

  // Vulkan Driver Info
  PATACHE_STRNCPY (pEngine->debugInfo.driverInfoVK, driver.driverInfo.data (),
                   VK_MAX_DRIVER_INFO_SIZE);

  // Vulkan Driver Version
  std::snprintf (pEngine->debugInfo.driverVersionVK, PATACHE_DRIVER_VERSION_VK_SIZE, "%u.%u.%u.%u",
                 VK_VERSION_MAJOR (physicalDeviceProperties.properties.driverVersion),
                 VK_VERSION_MINOR (physicalDeviceProperties.properties.driverVersion),
                 VK_VERSION_PATCH (physicalDeviceProperties.properties.driverVersion),
                 VK_API_VERSION_VARIANT (physicalDeviceProperties.properties.driverVersion));
#endif

  // VRAM HEAP size
  if (vramSizeTmp >= 1024U)
    {
#if PATACHE_DEBUG == 1
      pEngine->debugInfo.vramSizeUnitVK = "KiB";
      pEngine->debugInfo.vramSizeVK     = vramSizeTmp / 1024.0f;
#endif
      vramSizeUnit = "KiB";
      vramSize     = vramSizeTmp / 1024.0f;
    }

  if (vramSizeTmp >= 1024U)
    {
#if PATACHE_DEBUG == 1
      pEngine->debugInfo.vramSizeUnitVK = "MiB";
      pEngine->debugInfo.vramSizeVK     = vramSizeTmp / 1048576.0f;
#endif
      vramSizeUnit = "MiB";
      vramSize     = vramSizeTmp / 1048576.0f;
    }

  if (vramSizeTmp >= 1073741824U)
    {
#if PATACHE_DEBUG == 1
      pEngine->debugInfo.vramSizeUnitVK = "GiB";
      pEngine->debugInfo.vramSizeVK     = vramSizeTmp / 1073741824.0f;
#endif
      vramSizeUnit = "GiB";
      vramSize     = vramSizeTmp / 1073741824.0f;
    }

#if PATACHE_DEBUG == 1
  PATACHE_VARTYPE_STRING_PTR pVarTypeRealName = nullptr;
#endif

  // =================== Vulkan Version ===========================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, physicalDeviceProperties.properties.apiVersion);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 3, 2, 2),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Vulkan : ", PATACHE_TERM_RESET,
                        VK_VERSION_MAJOR (physicalDeviceProperties.properties.apiVersion), ".",
                        VK_VERSION_MINOR (physicalDeviceProperties.properties.apiVersion), ".",
                        VK_VERSION_PATCH (physicalDeviceProperties.properties.apiVersion), ".",
                        VK_API_VERSION_VARIANT (physicalDeviceProperties.properties.apiVersion),
                        "\n");

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Device ===================================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, physicalDeviceProperties);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 3, 2, 2),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Device", PATACHE_TERM_RESET);

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Device Name ==============================
  {
#if defined(PATACHE_DEBUG)
    PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, physicalDeviceProperties.properties.deviceName);
#endif

    fast_io::io::print (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Name : ", PATACHE_TERM_RESET);

#if defined(PATACHE_DEBUG)
    PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

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

#if defined(PATACHE_DEBUG)
    PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, physicalDeviceProperties.properties.vendorID);
#endif

    fast_io::io::print (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Vendor : ", PATACHE_TERM_RESET);

#if defined(PATACHE_DEBUG)
    PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

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
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, physicalDeviceProperties.properties.deviceType);
#endif

  // Device Type
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Type : ", PATACHE_TERM_RESET,
                        vk::to_string (physicalDeviceProperties.properties.deviceType));

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Device Vram Size =========================
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("Vram Size : ", 16), PATACHE_TERM_RESET, vramSize, " ",
                        fast_io::mnp::os_c_str (vramSizeUnit), "\n");

  // =================== Driver ===================================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, driver);
#endif

  // Vulkan Driver
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 3, 2, 2),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Driver", PATACHE_TERM_RESET);

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Driver Name ==============================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, driver.driverName);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Name : ", PATACHE_TERM_RESET,
                        fast_io::mnp::os_c_str (driver.driverName));

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Driver ID ================================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, driver.driverID);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "ID : ", PATACHE_TERM_RESET,
                        vk::to_string (driver.driverID));

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Driver Info ==============================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, driver.driverInfo);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Info : ", PATACHE_TERM_RESET,
                        fast_io::mnp::os_c_str (driver.driverInfo));

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Driver Version ===========================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, physicalDeviceProperties.properties.driverVersion);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Version : ", PATACHE_TERM_RESET,
                        VK_VERSION_MAJOR (physicalDeviceProperties.properties.driverVersion), ".",
                        VK_VERSION_MINOR (physicalDeviceProperties.properties.driverVersion), ".",
                        VK_VERSION_PATCH (physicalDeviceProperties.properties.driverVersion), ".",
                        VK_API_VERSION_VARIANT (physicalDeviceProperties.properties.driverVersion),
                        "\n");

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Swapchain ================================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, pEngine->vulkan.swapchain);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 3, 2, 2),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Swapchain", PATACHE_TERM_RESET);

#if PATACHE_DEBUG == 1
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Swapchain Present Mode ===================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, rSwapchainInfo.presentMode);
#endif

  // SwapChain Present Mode
  fast_io::io::print (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                      PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Present Mode : ", PATACHE_TERM_RESET,
                      vk::to_string (rSwapchainInfo.presentMode), " ");

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // Vulkan Vsync
  if ((rSwapchainInfo.presentMode == vk::PresentModeKHR::eFifo
       || rSwapchainInfo.presentMode == vk::PresentModeKHR::eFifoRelaxed)
      && pEngine->configuration.vsync)
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_COLOR_GREEN, "Vertical Sync",
                          PATACHE_TERM_RESET);
  else
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT);

  // =================== Swapchain Images =========================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, pEngine->vulkan.pSwapchainImages);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Image Count : ", PATACHE_TERM_RESET,
                        pEngine->vulkan.swapchainImageCount);

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Swapchain Image Color Format =============
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, rSwapchainInfo.imageColorFormat);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                        "Surface Color Format : ", PATACHE_TERM_RESET,
                        vk::to_string (rSwapchainInfo.imageColorFormat));

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Swapchain Image Color Format =============
  // =================== __cxa_demangle alloc =====================
#if defined(PATACHE_DEBUG)
  PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, rSwapchainInfo.imageColorSpace);
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if defined(PATACHE_DEBUG)
                        PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 5, 2, 4),
#endif
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                        "Surface Color Space : ", PATACHE_TERM_RESET,
                        vk::to_string (rSwapchainInfo.imageColorSpace), "\n");

#if defined(PATACHE_DEBUG)
  PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
#endif

  // =================== Debug ====================================
#if PATACHE_DEBUG == 1
#if defined(PATACHE_IMGUI_VERSION)
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::right ("ImGui Version : ", 18), PATACHE_TERM_RESET,
                        PATACHE_IMGUI_VERSION);
#endif
#endif

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT);
}
