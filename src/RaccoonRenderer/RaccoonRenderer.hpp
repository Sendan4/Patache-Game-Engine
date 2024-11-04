#pragma once

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <yaml-cpp/yaml.h>
#if defined(DEBUG)
#include <imgui_impl_vulkan.h>
#endif

#include "Config.hpp"
#if defined(DEBUG)
#include "EngineInfo.hpp"
#endif
#include "PatataEngine/PatataEngine.hpp"

namespace Patata
{
namespace Graphics
{
struct RaccoonRendererCreateInfo {
    Patata::Config * pConfiguration;
    SDL_Window ** ppWindow;
    bool * pWindowResized;
    Patata::ClearColor * pClearColor;
    #if defined(DEBUG)
    Patata::EngineInfo * pPatataEngineInfo;
    #endif
};

class RaccoonRenderer // Raccoon
{
public: // Raccoon Frontend
  RaccoonRenderer (RaccoonRendererCreateInfo *);
  ~RaccoonRenderer (void);

  bool BeginRender (SDL_Event &);
  void EndRender (SDL_Event &);

private:
  class VulkanBackend // Backend
  {
  public:
    VulkanBackend (RaccoonRendererCreateInfo *);
    ~VulkanBackend (void);

    // Backend Frontend
    bool BeginVulkanRender (SDL_Event &);
    void EndVulkanRender (SDL_Event &);

  private:
    RaccoonRendererCreateInfo * pRaccoonInfo = nullptr;
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
    bool CreateCommandPool(void);
    bool CreateCommandBuffer (void);
    bool CreateFrameBuffer (void);
    bool CreateDepthBuffer (void);
    bool CreateRenderPass (const std::tuple<vk::PresentModeKHR, vk::Format, vk::ColorSpaceKHR> &);
    bool CreatePipeline (void);
    bool CreateSemaphores(void);
    bool CreateFence(void);
    void RecreateSwapChain (SDL_Event &);

    #if defined(DEBUG)
    // Imgui
    bool VulkanImguiSetup (SDL_Window *);
    bool CreateImguiDescriptorPool(void);
    bool CreateImguiPipelineCache(void);
    #endif

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
    vk::CommandPool CommandPool;
    vk::CommandBuffer cmd;

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
  } * pVulkanBackend = nullptr; // End Vulkan Backend
}; // End Raccoon Renderer
}
} // End Patata Namespace
