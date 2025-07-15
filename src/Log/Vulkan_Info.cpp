#include "Vulkan_Info.hpp"

void
VulkanInfo (Patache::Engine * const        Engine,
            const Patache::SwapChainInfo & SwapChainInfo)
{
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                        PATACHE_TERM_COLOR_PATACHE, "Raccoon Renderer ",
                        PATACHE_TERM_RESET, "INFO");

  vk::PhysicalDeviceDriverProperties Driver;

  vk::PhysicalDeviceProperties2 PhysicalDeviceProperties;
  PhysicalDeviceProperties.pNext = &Driver;
  Engine->Vulkan.PhysicalDevice.getProperties2 (&PhysicalDeviceProperties);

#if PATACHE_DEBUG == 1
  /*
   * Storing information about Engine->Vulkan to display it through ImGui.
   * Only on debug builds
   */

  // Vulkan Version
  std::snprintf (
      Engine->engineInfo.VkVersion, PATACHE_VK_VERSION_SIZE, "%u.%u.%u.%u",
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.apiVersion),
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.apiVersion),
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.apiVersion),
      VK_API_VERSION_VARIANT (PhysicalDeviceProperties.properties.apiVersion));

  // Vulkan Device Name
  for (std::uint16_t i = 0; i < VK_MAX_PHYSICAL_DEVICE_NAME_SIZE; ++i)
    Engine->engineInfo.VkDeviceName[i]
        = *(PhysicalDeviceProperties.properties.deviceName.data () + i);

  // Vulkan Device Vendor ID
  Engine->engineInfo.VkDeviceVendorId
      = static_cast<uint32_t> (PhysicalDeviceProperties.properties.vendorID);

  // Vulkan Device Type
  PATACHE_STRNCPY (
      Engine->engineInfo.VkDeviceType,
      vk::to_string (PhysicalDeviceProperties.properties.deviceType).c_str (),
      PATACHE_VK_DEVICE_TYPE_SIZE);

  // Vulkan Driver Name
  for (std::uint16_t i = 0; i < VK_MAX_DRIVER_NAME_SIZE; ++i)
    Engine->engineInfo.VkDriverName[i] = *(Driver.driverName.data () + i);

  // Vulkan Driver ID
  PATACHE_STRNCPY (Engine->engineInfo.VkDriverId,
                   vk::to_string (Driver.driverID).c_str (),
                   PATACHE_VK_DRIVER_ID_SIZE);

  // Vulkan Driver Info
  for (std::uint16_t i = 0; i < VK_MAX_DRIVER_INFO_SIZE; ++i)
    Engine->engineInfo.VkDriverInfo[i] = *(Driver.driverInfo.data () + i);

  // Vulkan Driver Version
  std::snprintf (
      Engine->engineInfo.VkDriverVersion, PATACHE_VK_DRIVER_VERSION_SIZE,
      "%u.%u.%u.%u",
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.driverVersion),
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.driverVersion),
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.driverVersion),
      VK_API_VERSION_VARIANT (
          PhysicalDeviceProperties.properties.driverVersion));
#endif

  // =================== Vulkan Version ===========================
  // =================== __cxa_demangle alloc =====================
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                        "  Version", PATACHE_TERM_RESET);

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  char * VarTypeRealName = abi::__cxa_demangle (
      typeid (PhysicalDeviceProperties.properties.apiVersion).name (), nullptr,
      nullptr, nullptr);
#else
  const char * VarTypeRealName
      = typeid (PhysicalDeviceProperties.properties.apiVersion).name ();
#endif
#endif

  // Vulkan Version
  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else
      "    ",
#endif
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Vulkan : ", PATACHE_TERM_RESET,
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.apiVersion), ".",
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.apiVersion), ".",
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.apiVersion), ".",
      VK_API_VERSION_VARIANT (PhysicalDeviceProperties.properties.apiVersion),
      "\n");

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Device ===================================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (
      typeid (PhysicalDeviceProperties).name (), nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (PhysicalDeviceProperties).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 3),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "  ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Device", PATACHE_TERM_RESET);

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Device Name ==============================
  // =================== __cxa_demangle alloc =====================
  {
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
    // __cxa_demangle alloc
    VarTypeRealName = abi::__cxa_demangle (
        typeid (PhysicalDeviceProperties.properties.deviceName).name (),
        nullptr, nullptr, nullptr);
#else
    VarTypeRealName
        = typeid (PhysicalDeviceProperties.properties.deviceName).name ();
#endif
#endif

    fast_io::io::print (
        PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
        PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0,
        fast_io::mnp::right ("[", 5), fast_io::mnp::os_c_str (VarTypeRealName),
#else  // PATACHE_DEBUG
        "    ",
#endif // PATACHE_DEBUG
        PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Name : ", PATACHE_TERM_RESET);

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
    // free __cxa_demangle alloc
    std::free (VarTypeRealName);
#endif
#endif

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT,
                            PATACHE_TERM_COLOR_GREEN);
        break;
      }

    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT,
                          fast_io::mnp::os_c_str (
                              PhysicalDeviceProperties.properties.deviceName),
                          PATACHE_TERM_RESET);
  }

  // =================== Device Vendor ============================
  // =================== __cxa_demangle alloc =====================
  {
    const char * Vendor = nullptr;

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        Vendor = "Intel";
        break;

      case 4098:
        Vendor = "AMD";
        break;

      case 4318:
        Vendor = "Nvidia";
        break;

      case 65541:
        Vendor = "Mesa";
        break;

      default:
        Vendor = "Unknown";
        break;
      }

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
    // __cxa_demangle alloc
    VarTypeRealName = abi::__cxa_demangle (
        typeid (PhysicalDeviceProperties.properties.vendorID).name (), nullptr,
        nullptr, nullptr);
#else
    VarTypeRealName
        = typeid (PhysicalDeviceProperties.properties.vendorID).name ();
#endif
#endif

    fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
                        PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0,
                        fast_io::mnp::right ("[", 5),
                        fast_io::mnp::os_c_str (VarTypeRealName),
                        fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
                        "    ",
#endif // PATACHE_DEBUG
                        PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                        "Vendor : ", PATACHE_TERM_RESET);

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
    // free __cxa_demangle alloc
    std::free (VarTypeRealName);
#endif
#endif

    // String Vendor
    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_BLUE,
                            fast_io::mnp::os_c_str (Vendor),
                            PATACHE_TERM_RESET);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_RED,
                            fast_io::mnp::os_c_str (Vendor),
                            PATACHE_TERM_RESET);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_GREEN,
                            fast_io::mnp::os_c_str (Vendor),
                            PATACHE_TERM_RESET);
        break;

      case 65541:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_GRAY1,
                            fast_io::mnp::os_c_str (Vendor),
                            PATACHE_TERM_RESET);
        break;

      default:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT,
                            fast_io::mnp::os_c_str (Vendor));
        break;
      }

    // Hexadecimal VendorID
    fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_DIM,
                        PATACHE_TERM_COLOR_GRAY0, " | [Hexadecimal] ",
                        PATACHE_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT,
                            PATACHE_TERM_COLOR_GREEN);
        break;
      }

    fast_io::io::print (
        PATACHE_FAST_IO_BUFF_OUT,
        fast_io::mnp::hex (PhysicalDeviceProperties.properties.vendorID),
        PATACHE_TERM_RESET, " ");

    // Decimal VendorID
    fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_DIM,
                        PATACHE_TERM_COLOR_GRAY0, "| [Decimal] ",
                        PATACHE_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATACHE_FAST_IO_BUFF_OUT,
                            PATACHE_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT,
                          PhysicalDeviceProperties.properties.vendorID,
                          PATACHE_TERM_RESET);
  }

  // =================== Device Type ==============================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (
      typeid (PhysicalDeviceProperties.properties.deviceType).name (), nullptr,
      nullptr, nullptr);
#else
  VarTypeRealName
      = typeid (PhysicalDeviceProperties.properties.deviceType).name ();
#endif
#endif

  // Device Type
  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Type : ", PATACHE_TERM_RESET,
      vk::to_string (PhysicalDeviceProperties.properties.deviceType), "\n");

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Driver ===================================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (typeid (Driver).name (), nullptr,
                                         nullptr, nullptr);
#else
  VarTypeRealName = typeid (Driver).name ();
#endif
#endif

  // Vulkan Driver
  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 3),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "  ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Driver", PATACHE_TERM_RESET);

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Driver Name ==============================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (typeid (Driver.driverName).name (),
                                         nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (Driver.driverName).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Name : ", PATACHE_TERM_RESET,
      fast_io::mnp::os_c_str (Driver.driverName));

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Driver ID ================================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (typeid (Driver.driverID).name (),
                                         nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (Driver.driverID).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "ID : ", PATACHE_TERM_RESET,
      vk::to_string (Driver.driverID));

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Driver Info ==============================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (typeid (Driver.driverInfo).name (),
                                         nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (Driver.driverInfo).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Info : ", PATACHE_TERM_RESET,
      fast_io::mnp::os_c_str (Driver.driverInfo));

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Driver Version ===========================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (
      typeid (PhysicalDeviceProperties.properties.driverVersion).name (),
      nullptr, nullptr, nullptr);
#else
  VarTypeRealName
      = typeid (PhysicalDeviceProperties.properties.driverVersion).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Version : ", PATACHE_TERM_RESET,
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.driverVersion),
      ".",
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.driverVersion),
      ".",
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.driverVersion),
      ".",
      VK_API_VERSION_VARIANT (
          PhysicalDeviceProperties.properties.driverVersion),
      "\n");

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Swapchain ================================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  VarTypeRealName = abi::__cxa_demangle (
      typeid (Engine->Vulkan.SwapChain).name (), nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (Engine->Vulkan.SwapChain).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 3),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "  ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "SwapChain", PATACHE_TERM_RESET);

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Swapchain Present Mode ===================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName = abi::__cxa_demangle (
      typeid (SwapChainInfo.PresentMode).name (), nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (SwapChainInfo.PresentMode).name ();
#endif
#endif

  // SwapChain Present Mode
  fast_io::io::print (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
      "Present Mode : ", PATACHE_TERM_RESET,
      vk::to_string (SwapChainInfo.PresentMode), " ");

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // Vulkan Vsync
  if ((SwapChainInfo.PresentMode == vk::PresentModeKHR::eFifo
       || SwapChainInfo.PresentMode == vk::PresentModeKHR::eFifoRelaxed)
      && Engine->configuration.Vsync)
    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_COLOR_GREEN,
                          "Vertical Sync", PATACHE_TERM_RESET);
  else
    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);

  // =================== Swapchain Images =========================
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName
      = abi::__cxa_demangle (typeid (Engine->Vulkan.SwapChainImages).name (),
                             nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (Engine->Vulkan.SwapChainImages).name ();
#endif
#endif

  // Swapchain Images
  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD, "Images : ", PATACHE_TERM_RESET,
      Engine->Vulkan.SwapChainImageCount);

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Swapchain Image Color Format =============
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName
      = abi::__cxa_demangle (typeid (SwapChainInfo.ImageColorFormat).name (),
                             nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (SwapChainInfo.ImageColorFormat).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
      "Surface Color Format : ", PATACHE_TERM_RESET,
      vk::to_string (SwapChainInfo.ImageColorFormat));

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Swapchain Image Color Format =============
  // =================== __cxa_demangle alloc =====================
#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // __cxa_demangle alloc
  VarTypeRealName
      = abi::__cxa_demangle (typeid (SwapChainInfo.ImageColorSpace).name (),
                             nullptr, nullptr, nullptr);
#else
  VarTypeRealName = typeid (SwapChainInfo.ImageColorSpace).name ();
#endif
#endif

  fast_io::io::println (
      PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
      PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
      fast_io::mnp::os_c_str (VarTypeRealName), fast_io::mnp::left ("]", 2),
#else  // PATACHE_DEBUG
      "    ",
#endif // PATACHE_DEBUG
      PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
      "Surface Color Space : ", PATACHE_TERM_RESET,
      vk::to_string (SwapChainInfo.ImageColorSpace));

#if PATACHE_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  // free __cxa_demangle alloc
  std::free (VarTypeRealName);
#endif
#endif

  // =================== Debug ====================================
#if PATACHE_DEBUG == 1
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                        "\n  Debug", PATACHE_TERM_RESET);

// Vulkan Validation Layer Version
#if defined(PATACHE_USE_VVL)
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                        "    Validation Layers : ", PATACHE_TERM_RESET,
                        PATACHE_TERM_COLOR_GREEN, "Enabled",
                        PATACHE_TERM_RESET);
#endif

#if defined(PATACHE_IMGUI_VERSION)
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                        "    Imgui Version : ", PATACHE_TERM_RESET,
                        PATACHE_IMGUI_VERSION);
#endif
#endif // PATACHE_DEBUG
  fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);
}
