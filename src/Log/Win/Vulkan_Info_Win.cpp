#include "Vulkan_Info.hpp"

void
Patata::Graphics::RaccoonRenderer::VulkanBackend::VulkanInfo (
    YAML::Node CONFIG,
    std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR>
        SWAPCHAIN_INFO)
{
  HANDLE Terminal = GetStdHandle (STD_OUTPUT_HANDLE);
  DWORD  mode     = 0;
  GetConsoleMode (Terminal, &mode);
  SetConsoleMode (Terminal, ENABLE_VIRTUAL_TERMINAL_PROCESSING | mode);

  fast_io::io::println (fast_io::out(), "\n", PATATA_TERM_COLOR_PATATA,
                        "Raccoon Renderer ", PATATA_TERM_RESET, "INFO");

  vk::PhysicalDeviceDriverProperties Driver;

  vk::PhysicalDeviceProperties2 PhysicalDeviceProperties;
  PhysicalDeviceProperties.pNext = &Driver;
  PhysicalDevice.getProperties2 (&PhysicalDeviceProperties);

  const uint32_t VulkanVersion
      = PhysicalDeviceProperties.properties.apiVersion;

  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD, "  Version", PATATA_TERM_RESET);

// Vulkan Loader Version
#if defined(PATATA_VULKAN_LOADER_VERSION)
  if (strcmp (PATATA_VULKAN_LOADER_VERSION, "undefined") != 0)
    fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
                          "    Loader : ", PATATA_TERM_RESET, PATATA_VULKAN_LOADER_VERSION);
#endif

// Vulkan Headers Version
#if defined(PATATA_VULKAN_HEADERS_VERSION)
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
                        "    Headers : ", PATATA_TERM_RESET,
                        PATATA_VULKAN_HEADERS_VERSION);
#endif

  // Vulkan Version
  {
    fast_io::io::print (
        fast_io::out (), 
        PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.apiVersion).name (),
            nullptr, nullptr, nullptr) },
        "]",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.apiVersion).name () },
        "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " Vulkan : ", PATATA_TERM_RESET);

    std::string vk_version
        = std::to_string (VK_VERSION_MAJOR (VulkanVersion)) + '.'
          + std::to_string (VK_VERSION_MINOR (VulkanVersion)) + '.'
          + std::to_string (VK_VERSION_PATCH (VulkanVersion)) + '.'
          + std::to_string (VK_API_VERSION_VARIANT (VulkanVersion));

    fast_io::io::println (fast_io::out (), vk_version, "\n");
  }

  fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                        "  [",
                        std::string_view{ abi::__cxa_demangle (
                            typeid (PhysicalDeviceProperties).name (), nullptr,
                            nullptr, nullptr) },
                        "]",
#else
                         "  [",
                         std::string_view{
                             typeid (PhysicalDeviceProperties).name () },
                         "]",
#endif
                        PATATA_TERM_RESET, PATATA_TERM_BOLD, " Device", PATATA_TERM_RESET);

  // Device Name
  {
    fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.deviceName).name (),
            nullptr, nullptr, nullptr) },
        "]",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.deviceName).name () },
        "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " Name : ", PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_BLUE);
        break;
      case 4098:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_RED);
        break;
      case 4318:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::println (
        fast_io::out (), 
        std::string_view{ PhysicalDeviceProperties.properties.deviceName },
        PATATA_TERM_RESET);
  }

  // Vulkan Vendor String and ID
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
        fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.vendorID).name (),
            nullptr, nullptr, nullptr) },
        "]",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.vendorID).name () },
        "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " Vendor : ", PATATA_TERM_RESET);

    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_BLUE, Vendor,
                            PATATA_TERM_RESET);
        break;
      case 4098:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_RED, Vendor,
                            PATATA_TERM_RESET);
        break;
      case 4318:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GREEN, Vendor,
                            PATATA_TERM_RESET);
        break;
      case 65541:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY1, Vendor,
                            PATATA_TERM_RESET);
        break;
      default:
        fast_io::io::print (fast_io::out (), Vendor);
        break;
      }

    // Hexadecimal VendorID
    fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0,
                        " [Hexadecimal] ", PATATA_TERM_RESET);
    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_BLUE);
        break;
      case 4098:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_RED);
        break;
      case 4318:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::print (
        fast_io::out (),
        fast_io::mnp::hex (PhysicalDeviceProperties.properties.vendorID),
        PATATA_TERM_RESET, " ");

    // Decimal VendorID
    fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GRAY0, "[Decimal] ",
                        PATATA_TERM_RESET);
    switch (PhysicalDeviceProperties.properties.vendorID)
      {
      case 32902:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_BLUE);
        break;
      case 4098:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_RED);
        break;
      case 4318:
        fast_io::io::print (fast_io::out (), PATATA_TERM_COLOR_GREEN);
        break;
      }
    fast_io::io::println (fast_io::out (),
                          PhysicalDeviceProperties.properties.vendorID,
                          PATATA_TERM_RESET);
  }

  // Device Type
  {
    fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.deviceType).name (),
            nullptr, nullptr, nullptr) },
        "]",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.deviceType).name () },
        "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " Type : ", PATATA_TERM_RESET,
        vk::to_string (PhysicalDeviceProperties.properties.deviceType), "\n");
  }

  // Vulkan Driver
  fast_io::io::println (
      fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "  [",
      std::string_view{ abi::__cxa_demangle (typeid (Driver).name (), nullptr,
                                             nullptr, nullptr) },
      "]",
#else
      "  [", std::string_view{ typeid (Driver).name () }, "]",
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD, " Driver", PATATA_TERM_RESET);

  {
    fast_io::io::println (
        fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (Driver.driverName).name (), nullptr, nullptr, nullptr) },
        "]",
#else
        "    [", std::string_view{ typeid (Driver.driverName).name () }, "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD,
        " Name : ", PATATA_TERM_RESET,
        std::string_view{ Driver.driverName });

    fast_io::io::println (
        fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (Driver.driverID).name (), nullptr, nullptr, nullptr) },
        "]",
#else
        "    [", std::string_view{ typeid (Driver.driverID).name () }, "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " ID : ", PATATA_TERM_RESET,
        vk::to_string (Driver.driverID));

    fast_io::io::println (
        fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (Driver.driverInfo).name (), nullptr, nullptr, nullptr) },
        "]",
#else
        "    [", std::string_view{ typeid (Driver.driverInfo).name () }, "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " Info : ", PATATA_TERM_RESET,
        std::string_view{ Driver.driverInfo });
  }

  // Driver Version
  {
    fast_io::io::println (
        fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
        "    [",
        std::string_view{ abi::__cxa_demangle (
            typeid (PhysicalDeviceProperties.properties.driverVersion).name (),
            nullptr, nullptr, nullptr) },
        "]",
#else
        "    [",
        std::string_view{
            typeid (PhysicalDeviceProperties.properties.driverVersion)
                .name () },
        "]",
#endif
        PATATA_TERM_RESET, PATATA_TERM_BOLD, " Version : ", PATATA_TERM_RESET,
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
      fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
      "  [",
      std::string_view{ abi::__cxa_demangle (typeid (SwapChain).name (),
                                             nullptr, nullptr, nullptr) },
      "]",
#else
      "  [", std::string_view{ typeid (SwapChain).name () }, "]",
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD, " SwapChain" PATATA_TERM_RESET);

  // SwapChain Present Mode
  fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                        "    [",
                        std::string_view{ abi::__cxa_demangle (
                            typeid (std::get<0> (SWAPCHAIN_INFO)).name (),
                            nullptr, nullptr, nullptr) },
                        "]",
#else
                         "    [",
                         std::string_view{
                             typeid (std::get<0> (SWAPCHAIN_INFO)).name () },
                         "]",
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD, " Present Mode : ",
      PATATA_TERM_RESET, vk::to_string (std::get<0> (SWAPCHAIN_INFO)));

  // Vulkan Vsync
  {
    fast_io::io::print (fast_io::out (), PATATA_TERM_BOLD,
                        "    Vsync : " PATATA_TERM_RESET);

    if ((std::get<0> (SWAPCHAIN_INFO) == vk::PresentModeKHR::eMailbox
         || std::get<0> (SWAPCHAIN_INFO) == vk::PresentModeKHR::eFifo)
        && CONFIG["patata-engine"]["raccoon-renderer"]["vsync"].as<bool> ())
      fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GREEN, "Yes",
                            PATATA_TERM_RESET);
    else if (std::get<0> (SWAPCHAIN_INFO) == vk::PresentModeKHR::eImmediate
             && !CONFIG["patata-engine"]["raccoon-renderer"]["vsync"]
                     .as<bool> ())
      fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_YELLOW, "No",
                            PATATA_TERM_RESET);
  }

  // SwapChain Images
  fast_io::io::println (fast_io::out (), "    Images : ", SwapChainImageCount);

  // SwapChain Image Color Format
  fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                        "    [",
                        std::string_view{ abi::__cxa_demangle (
                            typeid (std::get<1> (SWAPCHAIN_INFO)).name (),
                            nullptr, nullptr, nullptr) },
                        "]",
#else
                         "    [",
                         std::string_view{
                             typeid (std::get<1> (SWAPCHAIN_INFO)).name () },
                         "]",
#endif
      PATATA_TERM_RESET, PATATA_TERM_BOLD,
                        " Image Color Format : ", PATATA_TERM_RESET,
                        vk::to_string (std::get<1> (SWAPCHAIN_INFO)));

  // SwapChain Image Color Space
  fast_io::io::println (fast_io::out (), PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
                        "    [",
                        std::string_view{ abi::__cxa_demangle (
                            typeid (std::get<2> (SWAPCHAIN_INFO)).name (),
                            nullptr, nullptr, nullptr) },
                        "]",
#else
                         "    [",
                         std::string_view{
                             typeid (std::get<2> (SWAPCHAIN_INFO)).name () },
                         "]",
#endif
                        PATATA_TERM_RESET, PATATA_TERM_BOLD,
                        " Image Color Space : ", PATATA_TERM_RESET,
                        vk::to_string (std::get<2> (SWAPCHAIN_INFO)), "\n");

  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD, "  Debug", PATATA_TERM_RESET);

#if defined(DEBUG)
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD, 
                        "    Validation Layer : ", PATATA_TERM_RESET,
                        PATATA_TERM_COLOR_GREEN, "Enabled", PATATA_TERM_RESET);

// Vulkan Validation Layer Version
#if defined(PATATA_VULKAN_VALIDATION_LAYERS_VERSION)
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
      "    Validation Layer Version : ", PATATA_TERM_RESET,
      PATATA_VULKAN_VALIDATION_LAYERS_VERSION);
#endif
#else
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
      "    Validation Layer : ", PATATA_TERM_RESET,
      PATATA_TERM_COLOR_YELLOW, "Disabled", PATATA_TERM_RESET);
#endif

#if defined(PATATA_IMGUI_VERSION)
  fast_io::io::println (fast_io::out (), PATATA_TERM_BOLD,
                        "    Imgui Version : ", PATATA_TERM_RESET,
                        PATATA_IMGUI_VERSION);
#endif
  fast_io::io::println (fast_io::out (), "");

  SetConsoleMode (Terminal, mode);
}
