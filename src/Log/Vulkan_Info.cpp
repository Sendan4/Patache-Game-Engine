#include "Vulkan_Info.hpp"

void
Patata::RaccoonRenderer::VulkanInfo (
    const Patata::SwapChainInfo & SwapChainInfo)
{
  fast_io::io::println ("\n", PATATA_TERM_BOLD, PATATA_TERM_COLOR_PATATA,
                        "Raccoon Renderer ", PATATA_TERM_RESET, "INFO");

  vk::PhysicalDeviceDriverProperties Driver;

  vk::PhysicalDeviceProperties2 PhysicalDeviceProperties;
  PhysicalDeviceProperties.pNext = &Driver;
  Vulkan.PhysicalDevice.getProperties2 (&PhysicalDeviceProperties);

#if PATATA_DEBUG == 1
  pRaccoonInfo->pPatataEngineInfo->VkVersionInUse
      = std::to_string (
            VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.apiVersion))
        + '.'
        + std::to_string (
            VK_VERSION_MINOR (PhysicalDeviceProperties.properties.apiVersion))
        + '.'
        + std::to_string (
            VK_VERSION_PATCH (PhysicalDeviceProperties.properties.apiVersion))
        + '.'
        + std::to_string (VK_API_VERSION_VARIANT (
            PhysicalDeviceProperties.properties.apiVersion));

  pRaccoonInfo->pPatataEngineInfo->VkDeviceName
      = PhysicalDeviceProperties.properties.deviceName.data ();
  pRaccoonInfo->pPatataEngineInfo->VkDeviceVendorId
      = static_cast<uint32_t> (PhysicalDeviceProperties.properties.vendorID);
  pRaccoonInfo->pPatataEngineInfo->VkDeviceType
      = vk::to_string (PhysicalDeviceProperties.properties.deviceType);
  pRaccoonInfo->pPatataEngineInfo->VkDriverName = Driver.driverName.data ();
  pRaccoonInfo->pPatataEngineInfo->VkDriverId
      = vk::to_string (Driver.driverID);
  pRaccoonInfo->pPatataEngineInfo->VkDriverInfo = Driver.driverInfo.data ();

  pRaccoonInfo->pPatataEngineInfo->VkDriverVersion
      = std::to_string (VK_VERSION_MAJOR (
            PhysicalDeviceProperties.properties.driverVersion))
        + '.'
        + std::to_string (VK_VERSION_MINOR (
            PhysicalDeviceProperties.properties.driverVersion))
        + '.'
        + std::to_string (VK_VERSION_PATCH (
            PhysicalDeviceProperties.properties.driverVersion))
        + '.'
        + std::to_string (VK_API_VERSION_VARIANT (
            PhysicalDeviceProperties.properties.driverVersion));
#endif

  fast_io::io::println (PATATA_TERM_BOLD, "  Version", PATATA_TERM_RESET);

// Vulkan Loader Version
#if defined(PATATA_VULKAN_LOADER_VERSION)
  if (strcmp (PATATA_VULKAN_LOADER_VERSION, "undefined") != 0)
    fast_io::io::println (PATATA_TERM_BOLD, "    Loader : ", PATATA_TERM_RESET,
                          PATATA_VULKAN_LOADER_VERSION);
#endif

// Vulkan Headers Version
#if defined(PATATA_VULKAN_HEADERS_VERSION)
  fast_io::io::println (PATATA_TERM_BOLD, "    Headers : ", PATATA_TERM_RESET,
                        PATATA_VULKAN_HEADERS_VERSION);
#endif

  // Vulkan Version
  {
    fast_io::io::print (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.apiVersion).name (),
            nullptr, nullptr, nullptr) },
        "] ",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.apiVersion).name () },
        "] ",
#endif
#else
        "    ",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Vulkan : ", PATATA_TERM_RESET);

    std::string_view vk_version{
      std::to_string (
          VK_VERSION_MAJOR (PhysicalDeviceProperties.properties.apiVersion))
      + '.'
      + std::to_string (
          VK_VERSION_MINOR (PhysicalDeviceProperties.properties.apiVersion))
      + '.'
      + std::to_string (
          VK_VERSION_PATCH (PhysicalDeviceProperties.properties.apiVersion))
      + '.'
      + std::to_string (VK_API_VERSION_VARIANT (
          PhysicalDeviceProperties.properties.apiVersion))
    };

    fast_io::io::println (vk_version, "\n");
  }

  fast_io::io::println (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "  [",
      std::string_view{
          abi::__cxa_demangle (typeid (PhysicalDeviceProperties).name (),
                               nullptr, nullptr, nullptr) },
      "] ",
#else
      "  [", std::string_view{ typeid (PhysicalDeviceProperties).name () },
      "] ",
#endif
#else  // PATATA_DEBUG
      "  ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Device", PATATA_TERM_RESET);

  // Device Name
  {
    fast_io::io::print (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.deviceName).name (),
            nullptr, nullptr, nullptr) },
        "] ",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.deviceName).name () },
        "] ",
#endif
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Name : ", PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_TERM_COLOR_BLUE);
        break;
      case 4098:
        fast_io::io::print (PATATA_TERM_COLOR_RED);
        break;
      case 4318:
        fast_io::io::print (PATATA_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::println (
        std::string_view{ PhysicalDeviceProperties.properties.deviceName },
        PATATA_TERM_RESET);
  }

  // Vulkan Vendor String
  {
    std::string Vendor;
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
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if PATATA_DEBUG == 1
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.vendorID).name (),
            nullptr, nullptr, nullptr) },
        "] ",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.vendorID).name () },
        "] ",
#endif
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Vendor : ", PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_TERM_COLOR_BLUE, Vendor, PATATA_TERM_RESET);
        break;
      case 4098:
        fast_io::io::print (PATATA_TERM_COLOR_RED, Vendor, PATATA_TERM_RESET);
        break;
      case 4318:
        fast_io::io::print (PATATA_TERM_COLOR_GREEN, Vendor,
                            PATATA_TERM_RESET);
        break;
      case 65541:
        fast_io::io::print (PATATA_TERM_COLOR_GRAY1, Vendor,
                            PATATA_TERM_RESET);
        break;
      default:
        fast_io::io::print (Vendor);
        break;
      }

    // Hexadecimal VendorID
    fast_io::io::print (PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
                        " | [Hexadecimal] ", PATATA_TERM_RESET);
    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_TERM_COLOR_BLUE);
        break;
      case 4098:
        fast_io::io::print (PATATA_TERM_COLOR_RED);
        break;
      case 4318:
        fast_io::io::print (PATATA_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::print (
        fast_io::mnp::hex (PhysicalDeviceProperties.properties.vendorID),
        PATATA_TERM_RESET, " ");

    // Decimal VendorID
    fast_io::io::print (PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
                        "| [Decimal] ", PATATA_TERM_RESET);
    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (PATATA_TERM_COLOR_BLUE);
        break;
      case 4098:
        fast_io::io::print (PATATA_TERM_COLOR_RED);
        break;
      case 4318:
        fast_io::io::print (PATATA_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::println (PhysicalDeviceProperties.properties.vendorID,
                          PATATA_TERM_RESET);
  }

  // Device Type
  {
    fast_io::io::println (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.deviceType).name (),
            nullptr, nullptr, nullptr) },
        "] ",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.deviceType).name () },
        "] ",
#endif
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Type : ", PATATA_TERM_RESET,
        vk::to_string (PhysicalDeviceProperties.properties.deviceType), "\n");
  }

  // Vulkan Driver
  fast_io::io::println (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "  [",
      std::string_view{ abi::__cxa_demangle (typeid (Driver).name (), nullptr,
                                             nullptr, nullptr) },
      "] ",
#else
      "  [", std::string_view{ typeid (Driver).name () }, "] ",
#endif
#else  // PATATA_DEBUG
      "  ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Driver", PATATA_TERM_RESET);

  {
    fast_io::io::println (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (Driver.driverName).name (), nullptr, nullptr, nullptr) },
        "] ",
#else
        "    [", std::string_view{ typeid (Driver.driverName).name () }, "] ",
#endif
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Name : ", PATATA_TERM_RESET,
        std::string_view{ Driver.driverName });

    fast_io::io::println (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (Driver.driverID).name (), nullptr, nullptr, nullptr) },
        "] ",
#else
        "    [", std::string_view{ typeid (Driver.ID).name () }, "] ",
#endif
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "ID : ", PATATA_TERM_RESET,
        vk::to_string (Driver.driverID));

    fast_io::io::println (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (Driver.driverInfo).name (), nullptr, nullptr, nullptr) },
        "] ",
#else
        "    [", std::string_view{ typeid (Driver.driverInfo).name () }, "] ",
#endif
#else  // PATATA_DEBUG
        "    ",
#endif // PATATA_DEBUG
        PATATA_TERM_RESET, PATATA_TERM_BOLD, "Info : ", PATATA_TERM_RESET,
        std::string_view{ Driver.driverInfo });
  }

  // Driver Version
  {
    fast_io::io::println (
#if PATATA_DEBUG == 1
        PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.driverVersion).name (),
            nullptr, nullptr, nullptr) },
        "] ",
#else
        "  [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.driverVersion)
                .name () },
        "] ",
#endif
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
  }

  fast_io::io::println (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "  [",
      std::string_view{ abi::__cxa_demangle (typeid (Vulkan.SwapChain).name (),
                                             nullptr, nullptr, nullptr) },
      "] ",
#else
      "  [", std::string_view{ typeid (Vulkan.SwapChain).name () }, "] ",
#endif
#else  // PATATA_DEBUG
      "  ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "SwapChain", PATATA_TERM_RESET);

  // SwapChain Present Mode
  fast_io::io::print (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "    [",
      std::string_view{
          abi::__cxa_demangle (typeid (SwapChainInfo.PresentMode).name (),
                               nullptr, nullptr, nullptr) },
      "] ",
#else
      "    [", std::string_view{ typeid (SwapChainInfo.PresentMode).name () },
      "] ",
#endif
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD,
      "Present Mode : ", PATATA_TERM_RESET,
      vk::to_string (SwapChainInfo.PresentMode), " ");

  // Vulkan Vsync
  if ((SwapChainInfo.PresentMode == vk::PresentModeKHR::eFifo
       || SwapChainInfo.PresentMode == vk::PresentModeKHR::eFifoRelaxed)
      && pRaccoonInfo->pConfiguration->Vsync)
    fast_io::io::println (PATATA_TERM_COLOR_GREEN, "Vertical Sync",
                          PATATA_TERM_RESET);
  else
    fast_io::io::println ("");

  // SwapChain Images
  fast_io::io::println (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "    [",
      std::string_view{
          abi::__cxa_demangle (typeid (Vulkan.SwapChainImages).name (),
                               nullptr, nullptr, nullptr) },
      "] ",
#else
      "    [", std::string_view{ typeid (Vulkan.SwapChainImages).name () },
      "] ",
#endif
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD, "Images : ", PATATA_TERM_RESET,
      Vulkan.SwapChainImageCount);

  // SwapChain Image Color Format
  fast_io::io::println (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "    [",
      std::string_view{
          abi::__cxa_demangle (typeid (SwapChainInfo.ImageColorFormat).name (),
                               nullptr, nullptr, nullptr) },
      "] ",
#else
      "    [",
      std::string_view{ typeid (SwapChainInfo.ImageColorFormat).name () },
      "] ",
#endif
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD,
      "Surface Color Format : ", PATATA_TERM_RESET,
      vk::to_string (SwapChainInfo.ImageColorFormat));

  // SwapChain Image Color Space
  fast_io::io::println (
#if PATATA_DEBUG == 1
      PATATA_TERM_DIM, PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "    [",
      std::string_view{
          abi::__cxa_demangle (typeid (SwapChainInfo.ImageColorSpace).name (),
                               nullptr, nullptr, nullptr) },
      "] ",
#else
      "    [",
      std::string_view{ typeid (SwapChainInfo.ImageColorSpace).name () }, "] ",
#endif
#else  // PATATA_DEBUG
      "    ",
#endif // PATATA_DEBUG
      PATATA_TERM_RESET, PATATA_TERM_BOLD,
      "Surface Color Space : ", PATATA_TERM_RESET,
      vk::to_string (SwapChainInfo.ImageColorSpace), "\n");

#if PATATA_DEBUG == 1
  fast_io::io::println (PATATA_TERM_BOLD, "  Debug", PATATA_TERM_RESET);

// Vulkan Validation Layer Version
#if defined(PATATA_USE_VVL)
  fast_io::io::println (PATATA_TERM_BOLD,
                        "    Validation Layers : ", PATATA_TERM_RESET,
                        PATATA_TERM_COLOR_GREEN, "Enabled", PATATA_TERM_RESET);
#endif

#if defined(PATATA_VULKAN_VALIDATION_LAYERS_VERSION)
  fast_io::io::println (PATATA_TERM_BOLD,
                        "    Validation Layers Version : ", PATATA_TERM_RESET,
                        PATATA_VULKAN_VALIDATION_LAYERS_VERSION);
#endif

#if defined(PATATA_IMGUI_VERSION)
  fast_io::io::println (PATATA_TERM_BOLD,
                        "    Imgui Version : ", PATATA_TERM_RESET,
                        PATATA_IMGUI_VERSION);
#endif
#endif // PATATA_DEBUG
  fast_io::io::println ("");
}
