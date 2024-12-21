#pragma once
namespace Patata
{
struct RaccoonRendererCreateInfo
{
    Patata::Config * pConfiguration;
    SDL_Window ** ppWindow;
    bool * pWindowResized;
    Patata::ClearColor * pClearColor;
    #if defined(DEBUG)
    Patata::EngineInfo * pPatataEngineInfo;
    #endif
};

class RaccoonRenderer
{
public: // Raccoon Frontend
  RaccoonRenderer(RaccoonRendererCreateInfo *);
  ~RaccoonRenderer (void);

  bool BeginRender (SDL_Event &);
  void EndRender (SDL_Event &);

private:
    RaccoonRendererCreateInfo * pRaccoonInfo = nullptr;

    // Begin Vulkan Backend
    VulkanBackend Vulkan;

    bool InitVulkanBackend(void);
    void CloseVulkanBackend(void);
    bool     CreateInstance (void);
    bool     SelectDevice (void);
    uint32_t CreateLogicalDeviceAndCreateQueue (void);
    bool CreateSwapChain (SwapChainInfo &);
    bool CreateImageView (const SwapChainInfo &);
    bool CreateCommandPool(void);
    bool CreateCommandBuffer (void);
    bool CreateFrameBuffer (void);
    bool CreateDepthBuffer (void);
    bool CreateRenderPass (const SwapChainInfo &);
    bool CreatePipeline (void);
    bool CreateSemaphores(void);
    bool CreateFence(void);
    void RecreateSwapChain (SDL_Event &);

    #if defined(DEBUG)
    // Imgui
    bool InitImguiVulkan (SDL_Window *);
    bool CreateImguiDescriptorPool(void);
    bool CreateImguiPipelineCache(void);
    #endif

    void
    VulkanInfo (const SwapChainInfo &);
    // End Vulkan Backend
}; // End Raccoon Renderer

} // End Patata Namespace
