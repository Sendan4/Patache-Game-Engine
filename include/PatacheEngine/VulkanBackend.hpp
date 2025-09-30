#pragma once
namespace Patache
{
struct VulkanBackend
{
  vk::Instance instance = VK_NULL_HANDLE;

  // Hardware
  vk::PhysicalDevice physicalDevice           = VK_NULL_HANDLE;
  std::uint32_t      graphicsQueueFamilyIndex = 0;
  vk::Queue          queue                    = VK_NULL_HANDLE;
  vk::Device         device                   = VK_NULL_HANDLE;

  // Window Surface
  vk::SurfaceKHR surface = VK_NULL_HANDLE;

  vk::SwapchainKHR swapchain    = VK_NULL_HANDLE;
  vk::SwapchainKHR oldSwapchain = VK_NULL_HANDLE;
  std::uint32_t    imageIndex   = 0;
  vk::Extent2D     swapchainExtent{};

  // Color
  std::uint32_t   swapchainImageCount       = 0;
  vk::Image *     pSwapchainImages          = nullptr;
  vk::ImageView * pSwapchainColorImageViews = nullptr;

  /*
  Depth buffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D
  performance. Depth Buffer can also be useful for the depth or priority of
  layers over others in 2D. To put it clearly, it is that one element is
  displayed correctly over another.
  */
  // Depth
  vk::Image        depthImage  = VK_NULL_HANDLE;
  vk::ImageView    depthView   = VK_NULL_HANDLE;
  vk::DeviceMemory depthMemory = VK_NULL_HANDLE;

  vk::Framebuffer * pSwapchainFrameBuffers = nullptr;

  // Commands
  vk::CommandPool     commandPool = VK_NULL_HANDLE;
  vk::CommandBuffer * pCmd        = nullptr;
  vk::CommandBuffer   cmdTransfer = VK_NULL_HANDLE;

  vk::RenderPass renderPass = VK_NULL_HANDLE;

  // Pipeline
  vk::PipelineLayout pipelineLayout        = VK_NULL_HANDLE;
  vk::Pipeline       graphicsPipeline      = VK_NULL_HANDLE;
  vk::PipelineCache  graphicsPipelineCache = VK_NULL_HANDLE;

#if PATACHE_DEBUG == 1
  vk::DebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

  // Imgui
  vk::PipelineLayout imguiPipeLineLayout = VK_NULL_HANDLE;
  vk::Pipeline       imguiPipeLine       = VK_NULL_HANDLE;

  vk::PipelineCache  imguiPipelineCache  = VK_NULL_HANDLE;
  vk::DescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;
#endif

  // Synchronization Primitives
  vk::Semaphore * pImageAvailableSemaphores = nullptr;
  vk::Semaphore * pImageFinishedSemaphores  = nullptr;
  vk::Fence *     pInFlightFences           = nullptr;

  std::uint8_t currentFrame = 0;

  // ClearColor
  vk::ClearValue clearColor{};

  // Render Command Buffer Area
  vk::Rect2D   renderArea{ vk::Offset2D{ 0, 0 } };
  vk::Viewport viewport{};
  vk::Rect2D   scissor{};

  // Buffer (GPU)
  vk::Buffer *       pRenderBuffer       = nullptr;
  vk::DeviceMemory * pRenderBufferMemory = nullptr;
  // Buffer (CPU) CPU -> GPU
  vk::Buffer       stagingBuffer       = VK_NULL_HANDLE;
  vk::DeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

  void * renderData = nullptr;
};

struct SwapchainInfo
{
  vk::PresentModeKHR presentMode;
  vk::Format         imageColorFormat = vk::Format::eUndefined;
  vk::ColorSpaceKHR  imageColorSpace;
};
}
