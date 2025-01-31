#pragma once
namespace Patata
{
struct VulkanBackend
{
  vk::Instance Instance = VK_NULL_HANDLE;

  // Hardware
  vk::PhysicalDevice PhysicalDevice           = VK_NULL_HANDLE;
  vk::Queue          Queue                    = VK_NULL_HANDLE;
  vk::Device         Device                   = VK_NULL_HANDLE;
  uint32_t           GraphicsQueueFamilyIndex = 0;

  // Window Surface
  vk::SurfaceKHR Surface = VK_NULL_HANDLE;

  vk::SwapchainKHR SwapChain    = VK_NULL_HANDLE;
  vk::SwapchainKHR OldSwapChain = VK_NULL_HANDLE;
  uint32_t         ImageIndex   = 0;
  vk::Extent2D     SwapChainExtent{};

  // Color
  uint32_t        SwapChainImageCount     = 0;
  vk::Image *     SwapChainImages         = VK_NULL_HANDLE;
  vk::ImageView * SwapChainColorImageView = VK_NULL_HANDLE;

  /*
  Depth buffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D
  performance. Depth Buffer can also be useful for the depth or priority of
  layers over others in 2D. To put it clearly, it is that one element is
  displayed correctly over another.
  */
  // Depth
  vk::Image        DepthImage  = VK_NULL_HANDLE;
  vk::ImageView    DepthView   = VK_NULL_HANDLE;
  vk::DeviceMemory DepthMemory = VK_NULL_HANDLE;

  // Commands
  vk::CommandPool   CommandPool = VK_NULL_HANDLE;
  vk::CommandBuffer cmd         = VK_NULL_HANDLE;

  vk::Framebuffer * SwapChainFrameBuffer = VK_NULL_HANDLE;

  vk::RenderPass RenderPass = VK_NULL_HANDLE;

#if PATATA_DEBUG == 1
  vk::DebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;

  // Imgui
  vk::PipelineLayout ImguiPipeLineLayout = VK_NULL_HANDLE;
  vk::Pipeline       ImguiPipeLine       = VK_NULL_HANDLE;

  vk::PipelineCache  ImguiPipelineCache  = VK_NULL_HANDLE;
  vk::DescriptorPool ImguiDescriptorPool = VK_NULL_HANDLE;
#endif

  // Synchronization Primitives
  vk::Semaphore AcquireSemaphore = VK_NULL_HANDLE;
  vk::Semaphore SubmitSemaphore  = VK_NULL_HANDLE;
  vk::Fence     Fence            = VK_NULL_HANDLE;
};

struct SwapChainInfo
{
  vk::PresentModeKHR PresentMode;
  vk::Format         ImageColorFormat = vk::Format::eUndefined;
  vk::ColorSpaceKHR  ImageColorSpace;
};
}
