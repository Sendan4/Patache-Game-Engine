#pragma once
namespace Patata
{
struct VulkanBackend {
    vk::Instance Instance = nullptr;

    // Hardware
    vk::PhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    vk::Queue          Queue = VK_NULL_HANDLE;
    vk::Device         Device         = VK_NULL_HANDLE;
    uint32_t GraphicsQueueFamilyIndex = 0;

    // Window Surface
    vk::SurfaceKHR Surface  = nullptr;

    vk::SwapchainKHR SwapChain = nullptr;
    vk::SwapchainKHR OldSwapChain = nullptr;
    uint32_t ImageIndex = 0;
    vk::Extent2D SwapChainExtent {};

    // Color
    uint32_t    SwapChainImageCount = 0;
    vk::Image * SwapChainImages = nullptr;
    vk::ImageView * SwapChainColorImageView = nullptr;

    /*
    Depth buffer will not be useful for now.
    It will be useful for advanced 2D effects and even more so for 3D performance.
    Depth Buffer can also be useful for the depth or priority of layers over others
    in 2D. To put it clearly, it is that one element is displayed correctly over another.
    */
    // Depth
    vk::Image     DepthImage = nullptr;
    vk::ImageView DepthView  = nullptr;
    vk::DeviceMemory DepthMemory = nullptr;

    // Commands
    vk::CommandPool CommandPool = nullptr;
    vk::CommandBuffer cmd = nullptr;

    vk::Framebuffer * SwapChainFrameBuffer = nullptr;

    vk::RenderPass RenderPass = nullptr;

    #if defined(DEBUG)
    // Imgui
    vk::PipelineLayout ImguiPipeLineLayout;
    vk::Pipeline ImguiPipeLine;

    vk::PipelineCache ImguiPipelineCache = nullptr;
    vk::DescriptorPool ImguiDescriptorPool = nullptr;

    ImGui_ImplVulkanH_Window ImGuiWindow;
    #endif

    // Synchronization Primitives
    vk::Semaphore AcquireSemaphore = nullptr;
    vk::Semaphore SubmitSemaphore = nullptr;
    vk::Fence     Fence = nullptr;
};

struct SwapChainInfo
{
    vk::PresentModeKHR PresentMode;
    vk::Format ImageColorFormat = vk::Format::eUndefined;
    vk::ColorSpaceKHR ImageColorSpace;
};
}
