#include "Setup_Vulkan_Renderer.hpp"

Patata::Graphics::RaccoonRenderer::VulkanBackend::VulkanBackend (
    SDL_Window * WINDOW, YAML::Node & CONFIG)
{
  if (!CreateInstance (WINDOW, CONFIG))
    return;

  // Search a Physical Device
  if (!SelectDevice (CONFIG))
    return;

  /*
  Search a queue of graphics from the selected physical device.
  Create a logical device from the selected physical device.
  */
  uint32_t GraphicsQueueFamilyIndex = CreateLogicalDeviceAndCreateQueue (CONFIG);

  // Create a surface for the window to draw on
  if (SDL_Vulkan_CreateSurface (WINDOW, Instance,
                                reinterpret_cast<VkSurfaceKHR *> (&Surface)))
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
#if !defined(_WIN64)
          PATATA_TERM_DIM,
#endif
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (Surface).name (),
                                                 nullptr, nullptr, nullptr) },
          "]",
#else
          "[", std::string_view{ typeid (Surface).name () }, "]",
#endif
          PATATA_TERM_RESET,
          PATATA_TERM_BOLD,
          " SDL Create Window Surface : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_GREEN, "Success", PATATA_TERM_RESET);
    }
  else
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
#if !defined(_WIN64)
          PATATA_TERM_DIM,
#endif
          PATATA_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
          "[",
          std::string_view{ abi::__cxa_demangle (typeid (Surface).name (),
                                                 nullptr, nullptr, nullptr) },
          "]",
#else
          "[", std::string_view{ typeid (Surface).name () }, "]",
#endif
          PATATA_TERM_RESET,
          PATATA_TERM_BOLD,
          " SDL Create Window Surface : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, "Fail", PATATA_TERM_RESET);

      Patata::Log::FatalErrorMessage ("SDL", SDL_GetError (), CONFIG);
      return;
    }

  std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> swapchaininfo
      = CreateSwapChain (GraphicsQueueFamilyIndex, CONFIG, WINDOW);

  vk::Result Result;

  // Color Image View
  {
      fast_io::io::println(
      #if defined(_WIN64)
          fast_io::out (),
      #endif
          PATATA_TERM_BOLD,
          "\nSwapChain Color Image Views :",
          PATATA_TERM_RESET);

      ColorView = new vk::ImageView[SwapChainImageCount];

      for (uint8_t i = 0; i < SwapChainImageCount; ++i) {
          vk::ImageViewCreateInfo ColorImageViewInfo {};
          ColorImageViewInfo.sType = vk::StructureType::eImageViewCreateInfo;
          ColorImageViewInfo.pNext = nullptr;
          ColorImageViewInfo.image = SwapChainImages[i];
          ColorImageViewInfo.viewType = vk::ImageViewType::e2D;
          ColorImageViewInfo.format = std::get<1>(swapchaininfo);
          ColorImageViewInfo.components.r = vk::ComponentSwizzle::eIdentity;
          ColorImageViewInfo.components.g = vk::ComponentSwizzle::eIdentity;
          ColorImageViewInfo.components.b = vk::ComponentSwizzle::eIdentity;
          ColorImageViewInfo.components.a = vk::ComponentSwizzle::eIdentity;
          ColorImageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
          ColorImageViewInfo.subresourceRange.baseMipLevel = 0;
          ColorImageViewInfo.subresourceRange.levelCount = 1;
          ColorImageViewInfo.subresourceRange.baseArrayLayer = 0;
          ColorImageViewInfo.subresourceRange.layerCount = 1;

          Result = Device.createImageView(&ColorImageViewInfo, nullptr, &ColorView[i]);
          {
            fast_io::io::print("  ");
            std::future<void> ReturnVulkanCheck = std::async (
                std::launch::async, Patata::Log::VulkanCheck, "Color Image View", Result);
          }
      }

      fast_io::io::println(
      #if defined(_WIN64)
          fast_io::out (),
      #endif
          "");
  }

  /*
  Depthbuffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D performance.
  */
  if (!CreateDepthBuffer(CONFIG)) return;

  // Command Pool
  {
    vk::CommandPoolCreateInfo CommandPoolInfo {};
    CommandPoolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    CommandPoolInfo.pNext = nullptr;
    CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    CommandPoolInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;

    Result = Device.createCommandPool (&CommandPoolInfo,
                                                  nullptr, &CommandPool);
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Command Pool", Result);
    }
  }

  // Fence
  {
    vk::FenceCreateInfo FenceInfo {};
    FenceInfo.sType = vk::StructureType::eFenceCreateInfo;
    FenceInfo.pNext = nullptr;
    FenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    Result = Device.createFence (&FenceInfo, nullptr, &Fence);
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patata::Log::VulkanCheck, "Fence", Result);
    }
  }

  // Semaphores
  {
      vk::SemaphoreCreateInfo SemaphoreInfo {};
      SemaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
      SemaphoreInfo.pNext = nullptr;

      Result = Device.createSemaphore (&SemaphoreInfo, nullptr, &AcquireSemaphore);
      {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patata::Log::VulkanCheck, "Acquire Next Image 2 KHR - Semaphore", Result);
      }

      Result = Device.createSemaphore (&SemaphoreInfo, nullptr, &SubmitSemaphore);
      {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patata::Log::VulkanCheck, "Submit Semaphore", Result);
      }
  }

  //CreateRenderPass ();

  VulkanInfo (CONFIG, swapchaininfo);
}

Patata::Graphics::RaccoonRenderer::VulkanBackend::~VulkanBackend (void)
{
  for (uint8_t i = 0; i < SwapChainImageCount; ++i)
    Device.destroyImageView(ColorView[i]);

  delete[] SwapChainImages;
  SwapChainImages = nullptr;

  Device.destroySemaphore (AcquireSemaphore);
  Device.destroySemaphore (SubmitSemaphore);

  #if defined(DEBUG)
  vk::Result Result = Device.waitForFences(1, &Fence, true, 0);
  {
	    std::future<void> ReturnVulkanCheck = std::async (
			std::launch::async, Patata::Log::VulkanCheck, "Wait For Fences", Result);
  }
  #endif
  Device.destroyFence (Fence);

  Device.destroyCommandPool (CommandPool);

  // Depth
  Device.freeMemory(DepthMemory);
  Device.destroyImageView(DepthView);
  Device.destroyImage(DepthImage);

  // Color

  Device.destroySwapchainKHR (SwapChain);

  Instance.destroySurfaceKHR (Surface);

  Device.destroy ();
  Instance.destroy ();
}
