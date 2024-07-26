#include "Setup_Vulkan_Renderer.hpp"

Patata::Graphics::RaccoonRenderer::VulkanBackend::VulkanBackend (
    SDL_Window * WINDOW, YAML::Node & CONFIG)
{
  if (!CreateInstance (WINDOW, CONFIG))
    return;

  if (!SelectDevice (CONFIG))
    return;

  uint32_t GraphicsQueueFamilyIndex = CreateLogicalDeviceAndCreateQueue (CONFIG);

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
#if !defined(_WIN64)
          PATATA_TERM_BOLD,
#endif
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
#if !defined(_WIN64)
          PATATA_TERM_BOLD,
#endif
          " SDL Create Window Surface : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, "Fail", PATATA_TERM_RESET);

      Patata::Log::FatalErrorMessage ("SDL", SDL_GetError (), CONFIG);
      return;
    }

  std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> swapchaininfo
      = CreateSwapChain (GraphicsQueueFamilyIndex, CONFIG, WINDOW);

  if (!CreateDepthBuffer(CONFIG)) return;

  // Command Pool
  {
    vk::CommandPoolCreateInfo CommandPoolInfo {};
    CommandPoolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    CommandPoolInfo.pNext = nullptr;
    CommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    CommandPoolInfo.queueFamilyIndex = GraphicsQueueFamilyIndex;

    vk::Result Result = Device.createCommandPool (&CommandPoolInfo,
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

    vk::Result Result = Device.createFence (&FenceInfo, nullptr, &Fence);
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

      vk::Result Result = Device.createSemaphore (&SemaphoreInfo, nullptr, &AcquireSemaphore);
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
  delete[] SwapChainImages;
  SwapChainImages = nullptr;

  Device.destroySemaphore (AcquireSemaphore);
  Device.destroySemaphore (SubmitSemaphore);
  vk::Result Result = Device.waitForFences(1, &Fence, true, 0);
  #if defined(DEBUG)
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
