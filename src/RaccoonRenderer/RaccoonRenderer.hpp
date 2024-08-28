#pragma once

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <yaml-cpp/yaml.h>

#include "Config.hpp"

namespace Patata
{
namespace Graphics
{
class RaccoonRenderer // Raccoon
{
public: // Raccoon Frontend
  RaccoonRenderer (Patata::Config &, SDL_Window *, bool &);
  ~RaccoonRenderer (void);

  void BeginRender(void);
  void EndRender(void);
  void ClearColor(const float & R, const float & G, const float & B, const float & A);

private:
  class VulkanBackend // Backend
  {
  public:
    VulkanBackend (Patata::Config &, SDL_Window *, bool &);
    ~VulkanBackend (void);

    void BeginVulkanRender (void);
    void EndVulkanRender (void);
    void VulkanClearColor(const float & R, const float & G, const float & B, const float & A);

  private:
    SDL_Window * pWindow = nullptr;
    bool * pWindowResized = nullptr;
    Patata::Config * pConfiguration = nullptr;
    bool CmdIsReady = false;

    /*
    These functions cover the creation or configuration of one or more
    elements for Vulkan that require at least some extensive writing of code.

    Those simple objects or elements are written directly to the constructor.
    */

    bool     CreateInstance (void);
    bool     SelectDevice (void);
    uint32_t CreateLogicalDeviceAndCreateQueue (void);
    bool CreateSwapChain (std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> &);
    bool CreateImageView (const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> &);
    bool CreateFrameBuffer (void);
    bool CreateDepthBuffer (void);
    bool CreateRenderPass (const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> &);
    void CreatePipeline (void);
    bool CreateSemaphores(void);
    bool CreateFence(void);
    void RecreateSwapChain(void);
    void
    VulkanInfo (const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> &);

    vk::Instance   Instance = nullptr;

    // Hardware
    vk::PhysicalDevice PhysicalDevice = nullptr;
    vk::Queue          Queue;
    vk::Device         Device         = nullptr;
    uint32_t GraphicsQueueFamilyIndex = 0;

    // Window Surface
    vk::SurfaceKHR Surface  = nullptr;

    vk::SwapchainKHR SwapChain = nullptr;
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
    vk::CommandPool CommandPool;
    vk::CommandBuffer cmd;

    vk::Framebuffer * SwapChainFrameBuffer = nullptr;

    vk::RenderPass RenderPass = nullptr;

    // Synchronization Primitives
    vk::Semaphore AcquireSemaphore = nullptr;
    vk::Semaphore SubmitSemaphore = nullptr;
    vk::Fence Fence = nullptr;

  } * pVulkanBackend = nullptr; // End Vulkan Backend
}; // End Raccoon Renderer
}
} // End Patata Namespace
