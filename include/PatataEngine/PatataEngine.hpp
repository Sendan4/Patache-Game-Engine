#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>
#if PATATA_DEBUG == 1
#include <SDL_syswm.h>
#endif

#include "PatataEngine/Libexport.h"
#include "PatataEngine/Color.hpp"
#include "PatataEngine/VulkanBackend.hpp"
#include "PatataEngine/StructConfig.hpp"
#include "PatataEngine/StructEngineInfo.hpp"

namespace Patata
{
enum class ViewMode : bool
{
  TwoDimensions,
  TreeDimensions
};

constexpr uint32_t
MakeGameVersion (const uint32_t & Major, const uint32_t & Minor,
                 const uint32_t & Patch, const uint32_t & Variant)
{
  return ((static_cast<uint32_t> (Variant)) << 29U
          | (static_cast<uint32_t> (Major)) << 22U
          | (static_cast<uint32_t> (Minor)) << 12U
          | static_cast<uint32_t> (Patch));
}

struct EngineCreateInfo
{
  const char *           gameName       = nullptr;
  const uint32_t         gameVersion    = 0; // TODO
  const char *           windowTitle    = nullptr;
  const Patata::ViewMode viewMode       = Patata::ViewMode::TwoDimensions;
  const char *           windowIconPath = nullptr;
};

struct ClearColor
{
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;
};

class Engine
{
public:
  PATATA_API Engine (void) = default;
  PATATA_API Engine (const Patata::EngineCreateInfo &, bool *);

  PATATA_API bool Init (const Patata::EngineCreateInfo &);

  PATATA_API ~Engine (void);

  Patata::Config configuration;

  PATATA_API void HandleEvent (const SDL_Event &); // Patata Events
  PATATA_API bool BeginRender (SDL_Event &);
  PATATA_API void EndRender (SDL_Event &);

  // Clear Color
  Patata::ClearColor clearColor{};

  PATATA_API void ClearColor (const Patata::Color &);
  PATATA_API void ClearColorRGBA (const float &, const float &, const float &,
                                  const float &);
  PATATA_API void ClearColorRGBA (const float &, const float &, const float &);

private:
  SDL_Window * GameWindow    = nullptr;
  bool         WindowResized = false;

  bool LoadConfiguration (void);
  void WindowLog (void);

  // Raccoon Renderer
  // Vulkan Backend
  Patata::VulkanBackend Vulkan;

  bool     RaccoonRendererInit (const Patata::EngineCreateInfo &);
  void     RaccoonRendererClose (void);
  bool     CreateInstance (const Patata::EngineCreateInfo &);
  bool     SelectDevice (void);
  uint32_t CreateLogicalDeviceAndCreateQueue (void);
  bool     CreateSwapChain (SwapChainInfo &);
  bool     CreateImageView (const SwapChainInfo &);
  bool     CreateCommandPool (void);
  bool     CreateCommandBuffer (void);
  bool     CreateFrameBuffer (void);
  bool     CreateDepthBuffer (void);
  bool     CreateRenderPass (const SwapChainInfo &);
  bool     CreatePipeline (void);
  bool     CreateSemaphores (void);
  bool     CreateFence (void);
  void     RecreateSwapChain (SDL_Event &);

#if PATATA_DEBUG == 1
  // Imgui
  void InitImgui (void);
  bool InitImguiVulkan (void);
  bool CreateImguiDescriptorPool (void);
  bool CreateImguiPipelineCache (void);

  Patata::EngineInfo engineInfo;
#endif

  void VulkanInfo (const SwapChainInfo &);
}; // End Engine Namespace
} // End Patata Namespace
