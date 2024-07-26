#pragma once

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <vulkan/vulkan.hpp>
#include <yaml-cpp/yaml.h>

namespace Patata
{
namespace Graphics
{
class RaccoonRenderer
{
public:
  RaccoonRenderer (YAML::Node &, SDL_Window *);
  ~RaccoonRenderer (void);

  void Render (void) { pVulkanBackend->VulkanRender(); }

private:
  class VulkanBackend
  {
  public:
    VulkanBackend (SDL_Window *, YAML::Node &);
    ~VulkanBackend (void);

    void VulkanRender (void);

  private:
    bool     CreateInstance (SDL_Window *&, YAML::Node &);
    bool     SelectDevice (YAML::Node &);
    uint32_t CreateLogicalDeviceAndCreateQueue (YAML::Node &);
    std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR>
         CreateSwapChain (uint32_t &, YAML::Node, SDL_Window *);
    bool CreateDepthBuffer (YAML::Node &);
    void CreateRenderPass (void);
    void CreatePipeline (void);
    void
    VulkanInfo (YAML::Node CONFIG,
                std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR>);

    vk::Instance   Instance = nullptr;
    vk::PhysicalDevice PhysicalDevice = nullptr;
    vk::Queue          Queue;
    vk::Device         Device         = nullptr;

    vk::SurfaceKHR Surface  = nullptr;

    vk::SwapchainKHR SwapChain = nullptr;
    vk::Extent2D SwapChainExtent {};

    // Color
    uint32_t    SwapChainImageCount = 0;
    vk::Image * SwapChainImages = nullptr;

    // Depth
    vk::Image     DepthImage = nullptr;
    vk::ImageView DepthView  = nullptr;
    vk::DeviceMemory DepthMemory = nullptr;

    vk::CommandPool CommandPool;
    vk::CommandBuffer cmd;

    vk::RenderPass RenderPass = nullptr;
    vk::Semaphore AcquireSemaphore = nullptr;
    vk::Semaphore SubmitSemaphore = nullptr;
    vk::Fence Fence = nullptr;

    vk::Pipeline       PipeLine;
    vk::PipelineLayout PipeLineLayout;
    vk::PipelineCache  PipeLineCache;
  } * pVulkanBackend = nullptr;
};
}
}
