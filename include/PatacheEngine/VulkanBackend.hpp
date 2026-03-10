#pragma once
namespace Patache
{
struct VulkanBackend
{
  VkInstance instance{ VK_NULL_HANDLE };

  // Hardware
  VkPhysicalDevice     physicalDevice{ VK_NULL_HANDLE };
  std::uint32_t        graphicsQueueFamilyIndex{ 0U };
  VkQueue              queue{ VK_NULL_HANDLE };
  VkDevice             device{ VK_NULL_HANDLE };
  VkPhysicalDeviceType physicalDeviceType{ VK_PHYSICAL_DEVICE_TYPE_OTHER };

  // Window Surface
  VkSurfaceKHR surface{ VK_NULL_HANDLE };

  VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
  VkSwapchainKHR oldSwapchain{ VK_NULL_HANDLE };
  std::uint32_t  imageIndex{ 0U };
  VkExtent2D     swapchainExtent{};

  // Color
  std::uint32_t swapchainImageCount{ 0U };
  VkImage *     pSwapchainImages{ nullptr };
  VkImageView * pSwapchainColorImageViews{ nullptr };

  /*
  Depth buffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D
  performance. Depth Buffer can also be useful for the depth or priority of
  layers over others in 2D. To put it clearly, it is that one element is
  displayed correctly over another.
  */
  // Depth
  VkImage        depthImage{ VK_NULL_HANDLE };
  VkImageView    depthView{ VK_NULL_HANDLE };
  VkDeviceMemory depthMemory{ VK_NULL_HANDLE };

  VkFramebuffer * pSwapchainFrameBuffers{ nullptr };

  // Commands
  VkCommandPool *   pCommandPools{ nullptr };
  VkCommandBuffer * pCmd{ nullptr };

  VkRenderPass renderPass{ VK_NULL_HANDLE };

  // Pipeline
  VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
  VkPipeline       graphicsPipeline{ VK_NULL_HANDLE };
  VkPipelineCache  graphicsPipelineCache{ VK_NULL_HANDLE };

#if PATACHE_DEBUG == 1
  VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };

  // Imgui
  VkPipelineLayout imguiPipeLineLayout{ VK_NULL_HANDLE };
  VkPipeline       imguiPipeLine{ VK_NULL_HANDLE };

  VkPipelineCache  imguiPipelineCache{ VK_NULL_HANDLE };
  VkDescriptorPool imguiDescriptorPool{ VK_NULL_HANDLE };
#endif

  // Synchronization Primitives
  VkSemaphore * pImageAvailableSemaphores{ nullptr };
  VkSemaphore * pImageFinishedSemaphores{ nullptr };
  VkFence *     pInFlightFences{ nullptr };

  std::uint8_t currentFrame{ 0U };

  // ClearColor
  VkClearValue clearColor{};

  // Render Command Buffer Area
  VkRect2D   renderArea{};
  VkViewport viewport{};
  VkRect2D   scissor{};

  VmaAllocator allocator;

  // Buffer (GPU)
  VkBuffer *      pRenderBuffer{ nullptr };
  VmaAllocation * pRenderAllocation{ nullptr };
  VkDeviceSize    renderBufferInfo[2U]{ 0U, 0U };
  // Buffer (CPU) CPU -> GPU
  VkBuffer      stagingBuffer{ VK_NULL_HANDLE };
  VmaAllocation stagingAllocation;
  VkDeviceSize  stagingBufferInfo[2U]{ 0U, 0U };

  VkBufferCopy          bufferCopy{};
  VkBufferMemoryBarrier bufferBarrier{ .sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                                       .pNext         = nullptr,
                                       .srcAccessMask = 0U,
                                       .dstAccessMask = 0U,
                                       .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                       .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                       .buffer              = VK_NULL_HANDLE,
                                       .offset              = 0U,
                                       .size                = 0U };

  std::uint8_t copiesCount{ 0U };

  VmaPool renderPool;

  VkResult renderResult;
};

enum VkBufferInfo : std::uint_fast8_t
{
  eSize          = 0U,
  eCurrentOffset = 1U
};

struct SwapchainInfo
{
  VkPresentModeKHR presentMode;
  VkFormat         imageColorFormat{ VK_FORMAT_UNDEFINED };
  VkColorSpaceKHR  imageColorSpace;
};
}
