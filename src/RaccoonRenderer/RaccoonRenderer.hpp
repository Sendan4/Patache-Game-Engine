#pragma once

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>
#include <yaml-cpp/yaml.h>
#include <fast_io.h>

namespace Patata
{
namespace Graphics
{
// Raccoon
class RaccoonRenderer
{
public:
  // Raccoon Frontend
  RaccoonRenderer (YAML::Node &, SDL_Window *);
  ~RaccoonRenderer (void);

  /*
  The rendering process of each backend must pass through here,
  each one will be called according to the configuration.
  THIS MAY CHANGE IN THE FUTURE.
  */
  void Render (void) { pVulkanBackend->VulkanRender(); }

private:
  // Backend
  class VulkanBackend
  {
  public:
    // Frontend
    VulkanBackend (SDL_Window *, YAML::Node &);
    ~VulkanBackend (void);

    void VulkanRender (void);

  private:
    // Backend

    /*
    These functions cover the creation or configuration of one or more
    elements for Vulkan that require at least some extensive writing of code.

    Those simple objects or elements are written directly to the constructor.
    */

    bool     CreateInstance (SDL_Window *&, YAML::Node &);
    bool     SelectDevice (YAML::Node &);
    uint32_t CreateLogicalDeviceAndCreateQueue (YAML::Node &);
    std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR>
         CreateSwapChain (uint32_t &, YAML::Node, SDL_Window *);
    bool CreateDepthBuffer (YAML::Node &);
    bool CreateRenderPass (
        const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> &);
    void CreatePipeline (void);
    void
    VulkanInfo (YAML::Node CONFIG,
                std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR>);

    vk::Instance   Instance = nullptr;

    // Hardware
    vk::PhysicalDevice PhysicalDevice = nullptr;
    vk::Queue          Queue;
    vk::Device         Device         = nullptr;

    // Window Surface
    vk::SurfaceKHR Surface  = nullptr;

    vk::SwapchainKHR SwapChain = nullptr;
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

    //vk::Pipeline       PipeLine;
    //vk::PipelineLayout PipeLineLayout;
    //vk::PipelineCache  PipeLineCache;

  } * pVulkanBackend = nullptr;
};
}
}
