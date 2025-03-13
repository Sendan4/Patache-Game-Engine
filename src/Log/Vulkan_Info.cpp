#include "Vulkan_Info.hpp"

void
Patata::Engine::VulkanInfo (const Patata::SwapChainInfo & SwapChainInfo)
{
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                        PATATA_TERM_COLOR_PATATA, "Raccoon Renderer ",
                        PATATA_TERM_RESET, "INFO");

  vk::PhysicalDeviceDriverProperties Driver;

  vk::PhysicalDeviceProperties2 PhysicalDeviceProperties;
  PhysicalDeviceProperties.pNext = &Driver;
  Vulkan.PhysicalDevice.getProperties2 (&PhysicalDeviceProperties);

#if PATATA_DEBUG == 1
  /*
   * Storing information about Vulkan to display it through ImGui.
   * Only on debug builds
   */

  // Vulkan Version
  std::snprintf (
      engineInfo.VkVersion, PATATA_VK_VERSION_SIZE, "%u.%u.%u.%u",
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.apiVersion),
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.apiVersion),
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.apiVersion),
      VK_API_VERSION_VARIANT (PhysicalDeviceProperties.properties.apiVersion));

  // Vulkan Device Name
  for (std::uint16_t i = 0; i < VK_MAX_PHYSICAL_DEVICE_NAME_SIZE; ++i)
    engineInfo.VkDeviceName[i]
        = *(PhysicalDeviceProperties.properties.deviceName.data () + i);

  // Vulkan Device Vendor ID
  engineInfo.VkDeviceVendorId
      = static_cast<uint32_t> (PhysicalDeviceProperties.properties.vendorID);

  // Vulkan Device Type
  PATATA_STRNCPY (
      engineInfo.VkDeviceType,
      vk::to_string (PhysicalDeviceProperties.properties.deviceType).c_str (),
      PATATA_VK_DEVICE_TYPE_SIZE);

  // Vulkan Driver Name
  for (std::uint16_t i = 0; i < VK_MAX_DRIVER_NAME_SIZE; ++i)
    engineInfo.VkDriverName[i] = *(Driver.driverName.data () + i);

  // Vulkan Driver ID
  PATATA_STRNCPY (engineInfo.VkDriverId,
                  vk::to_string (Driver.driverID).c_str (),
                  PATATA_VK_DRIVER_ID_SIZE);

  // Vulkan Driver Info
  for (std::uint16_t i = 0; i < VK_MAX_DRIVER_INFO_SIZE; ++i)
    engineInfo.VkDriverInfo[i] = *(Driver.driverInfo.data () + i);

  // Vulkan Driver Version
  std::snprintf (
      engineInfo.VkDriverVersion, PATATA_VK_DRIVER_VERSION_SIZE, "%u.%u.%u.%u",
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.driverVersion),
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.driverVersion),
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.driverVersion),
      VK_API_VERSION_VARIANT (
          PhysicalDeviceProperties.properties.driverVersion));
#endif

  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD, "  Version",
                        PATATA_TERM_RESET);

// Vulkan Loader Version
#if defined(PATATA_VULKAN_LOADER_VERSION)
  if (std::strcmp (PATATA_VULKAN_LOADER_VERSION, "undefined") != 0)
    fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                          fast_io::mnp::right ("Loader : ", 13),
                          PATATA_TERM_RESET, PATATA_VULKAN_LOADER_VERSION);
#endif

// Vulkan Headers Version
#if defined(PATATA_VULKAN_HEADERS_VERSION)
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                        fast_io::mnp::right ("    Headers : ", 14),
                        PATATA_TERM_RESET, PATATA_VULKAN_HEADERS_VERSION);
#endif

  // Vulkan Version
  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (PhysicalDeviceProperties.properties.apiVersion).name (),
          nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (
          typeid (PhysicalDeviceProperties.properties.apiVersion).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else
      "    ",
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Vulkan : ", PATATA_TERM_RESET,
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.apiVersion), ".",
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.apiVersion), ".",
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.apiVersion), ".",
      VK_API_VERSION_VARIANT (PhysicalDeviceProperties.properties.apiVersion),
      "\n");

  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 3),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (
          abi::__cxa_demangle (typeid (PhysicalDeviceProperties).name (),
                               nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (PhysicalDeviceProperties).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "  ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Device", PATATA_TERM_RESET);

  // Device Name
  {
    fast_io::io::print (
        PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        fast_io::mnp::os_c_str (abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.deviceName).name (),
            nullptr, nullptr, nullptr)),
#else
        fast_io::mnp::os_c_str (
            typeid (PhysicalDeviceProperties.properties.deviceName).name ()),
#endif
        fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Name : ", PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_GREEN);
        break;
      }

    fast_io::io::println (PATATA_FAST_IO_BUFF_OUT,
                          fast_io::mnp::os_c_str (
                              PhysicalDeviceProperties.properties.deviceName),
                          PATATA_TERM_RESET);
  }

  // Vulkan Vendor String
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

    fast_io::io::print (
        PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        fast_io::mnp::os_c_str (abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.vendorID).name (),
            nullptr, nullptr, nullptr)),
#else
        fast_io::mnp::os_c_str (
            typeid (PhysicalDeviceProperties.properties.vendorID).name ()),
#endif
        fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Vendor : ", PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_BLUE,
                            fast_io::mnp::os_c_str (Vendor),
                            PATATA_TERM_RESET);
        break;

      case 4098:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_RED,
                            fast_io::mnp::os_c_str (Vendor),
                            PATATA_TERM_RESET);
        break;

      case 4318:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_GREEN,
                            fast_io::mnp::os_c_str (Vendor),
                            PATATA_TERM_RESET);
        break;

      case 65541:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_GRAY1,
                            fast_io::mnp::os_c_str (Vendor),
                            PATATA_TERM_RESET);
        break;

      default:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT,
                            fast_io::mnp::os_c_str (Vendor));
        break;
      }

    // Hexadecimal VendorID
    fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_DIM,
                        PATATA_TERM_COLOR_GRAY0, " | [Hexadecimal] ",
                        PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_GREEN);
        break;
      }

    fast_io::io::print (
        PATATA_FAST_IO_BUFF_OUT,
        fast_io::mnp::hex (PhysicalDeviceProperties.properties.vendorID),
        PATATA_TERM_RESET, " ");

    // Decimal VendorID
    fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_DIM,
                        PATATA_TERM_COLOR_GRAY0, "| [Decimal] ",
                        PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_BLUE);
        break;

      case 4098:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_RED);
        break;

      case 4318:
        fast_io::io::print (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::println (PATATA_FAST_IO_BUFF_OUT,
                          PhysicalDeviceProperties.properties.vendorID,
                          PATATA_TERM_RESET);
  }

  // Device Type
  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (PhysicalDeviceProperties.properties.deviceType).name (),
          nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (
          typeid (PhysicalDeviceProperties.properties.deviceType).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Type : ", PATATA_TERM_RESET,
      vk::to_string (PhysicalDeviceProperties.properties.deviceType), "\n");

  // Vulkan Driver
  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 3),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (typeid (Driver).name (),
                                                   nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (Driver).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "  ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Driver", PATATA_TERM_RESET);

  // Driver Name
  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (Driver.driverName).name (), nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (Driver.driverName).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Name : ", PATATA_TERM_RESET,
      fast_io::mnp::os_c_str (Driver.driverName), "\n",
// End Driver Name
// Driver ID
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (Driver.driverID).name (), nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (Driver.driverID).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "ID : ", PATATA_TERM_RESET,
      vk::to_string (Driver.driverID), "\n",
// End Driver ID
// Driver Info
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (Driver.driverInfo).name (), nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (Driver.driverInfo).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Info : ", PATATA_TERM_RESET,
      fast_io::mnp::os_c_str (Driver.driverInfo), "\n",
// End Driver Info
// Driver Version
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (PhysicalDeviceProperties.properties.driverVersion).name (),
          nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (
          typeid (PhysicalDeviceProperties.properties.driverVersion).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Version : ", PATATA_TERM_RESET,
      VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.driverVersion),
      ".",
      VK_VERSION_MINOR (PhysicalDeviceProperties.properties.driverVersion),
      ".",
      VK_VERSION_PATCH (PhysicalDeviceProperties.properties.driverVersion),
      ".",
      VK_API_VERSION_VARIANT (
          PhysicalDeviceProperties.properties.driverVersion),
      "\n");
  // End Driver Version

  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 3),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (Vulkan.SwapChain).name (), nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (Vulkan.SwapChain).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "  ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "SwapChain", PATATA_TERM_RESET);

  // SwapChain Present Mode
  fast_io::io::print (PATATA_FAST_IO_BUFF_OUT,
#if PATATA_DEBUG == 1
                      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
                      fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                      fast_io::mnp::os_c_str (abi::__cxa_demangle (
                          typeid (SwapChainInfo.PresentMode).name (), nullptr,
                          nullptr, nullptr)),
#else
                      fast_io::mnp::os_c_str (
                          typeid (SwapChainInfo.PresentMode).name ()),
#endif
                      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
                      "    ",
#endif // PATATA_DEBUG
                      PATATA_TERM_RESET, PATATA_TERM_BOLD,
                      "Present Mode : ", PATATA_TERM_RESET,
                      vk::to_string (SwapChainInfo.PresentMode), " ");

  // Vulkan Vsync
  if ((SwapChainInfo.PresentMode == vk::PresentModeKHR::eFifo
       || SwapChainInfo.PresentMode == vk::PresentModeKHR::eFifoRelaxed)
      && configuration.Vsync)
    fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_COLOR_GREEN,
                          "Vertical Sync", PATATA_TERM_RESET);
  else
    fast_io::io::println (PATATA_FAST_IO_BUFF_OUT);

  fast_io::io::println (
      PATATA_FAST_IO_BUFF_OUT,
  // SwapChain Image Count
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (abi::__cxa_demangle (
          typeid (Vulkan.SwapChainImages).name (), nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (Vulkan.SwapChainImages).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Images : ", PATATA_TERM_RESET,
      Vulkan.SwapChainImageCount, "\n",
// End SwapChain Image Count
// SwapChain Image Color Format
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (
          abi::__cxa_demangle (typeid (SwapChainInfo.ImageColorFormat).name (),
                               nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (SwapChainInfo.ImageColorFormat).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD,
      "Surface Color Format : ", PATATA_TERM_RESET,
      vk::to_string (SwapChainInfo.ImageColorFormat), "\n",
// End SwapChain Image Color Format
// SwapChain Image Color Space
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0, fast_io::mnp::right ("[", 5),
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      fast_io::mnp::os_c_str (
          abi::__cxa_demangle (typeid (SwapChainInfo.ImageColorSpace).name (),
                               nullptr, nullptr, nullptr)),
#else
      fast_io::mnp::os_c_str (typeid (SwapChainInfo.ImageColorSpace).name ()),
#endif
      fast_io::mnp::left ("]", 2),
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD,
      "Surface Color Space : ", PATATA_TERM_RESET,
      vk::to_string (SwapChainInfo.ImageColorSpace));
  // End SwapChain Image Color Space

#if PATATA_DEBUG == 1
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD, "\n  Debug",
                        PATATA_TERM_RESET);

// Vulkan Validation Layer Version
#if defined(PATATA_USE_VVL)
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                        "    Validation Layers : ", PATATA_TERM_RESET,
                        PATATA_TERM_COLOR_GREEN, "Enabled", PATATA_TERM_RESET);
#endif

#if defined(PATATA_VULKAN_VALIDATION_LAYERS_VERSION)
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                        "    Validation Layers Version : ", PATATA_TERM_RESET,
                        PATATA_VULKAN_VALIDATION_LAYERS_VERSION);
#endif

#if defined(PATATA_IMGUI_VERSION)
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT, PATATA_TERM_BOLD,
                        "    Imgui Version : ", PATATA_TERM_RESET,
                        PATATA_IMGUI_VERSION);
#endif
#endif // PATATA_DEBUG
  fast_io::io::println (PATATA_FAST_IO_BUFF_OUT);
}
