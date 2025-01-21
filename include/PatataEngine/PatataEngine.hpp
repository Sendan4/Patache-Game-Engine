#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "PatataEngine/Libexport.h"
#include "PatataEngine/Color.hpp"
#include "PatataEngine/VulkanBackend.hpp"
#include "PatataEngine/StructConfig.hpp"
#include "PatataEngine/StructEngineInfo.hpp"

namespace Patata
{
// Versioning management in the style of Vulkan. // major.minor.patch.variant
constexpr uint32_t
MakeVersion (const uint8_t & Major, const uint16_t & Minor,
             const uint16_t & Patch, const uint8_t & Variant)
{
  return ((static_cast<uint32_t> (Variant)) << 29U
          | (static_cast<uint32_t> (Major)) << 22U
          | (static_cast<uint32_t> (Minor)) << 12U
          | static_cast<uint32_t> (Patch));
}

constexpr uint8_t
GetVersionMajor (const uint32_t & Major)
{
  // 6 bits
  return Major >> 22U & 0b1111111;
}

constexpr uint16_t
GetVersionMinor (const uint32_t & Minor)
{
  // 9 bits
  return Minor >> 12U & 0b1111111111;
}

constexpr uint16_t
GetVersionPatch (const uint32_t & Patch)
{
  // 11 bits
  return Patch & 0b111111111111;
}

constexpr uint8_t
GetVersionVariant (const uint32_t & Variant)
{
  // 5 bits
  return Variant >> 29U & 0b111111;
}

enum class ViewMode : bool
{
  TwoDimensions, // 2D
  TreeDimensions // 3D
};

// you can use nullptr to not use the options or not assign them at all.
struct EngineCreateInfo
{
  const char * gameName = nullptr; // Name of the game // It is optional.
  const uint32_t gameVersion = 0; // Vulkan version style: major.minor.patch.variant // It is optional.
  const char * windowTitle = nullptr; // Initial title of the window. // It is optional.
  const Patata::ViewMode viewMode = Patata::ViewMode::TwoDimensions;
  // Determines whether the project uses 2D or 3D graphics.
  // It is mandatory.
  const char * windowIconPath = nullptr; // Initial icon of the window. // It is optional.
  // Use bitmap (.bmp) format/codec. // It is optional.
};

// Float format
struct ClearColor
{
  float r = 0.0f; // Red
  float g = 0.0f; // Green
  float b = 0.0f; // Blue
  float a = 1.0f; // Alpha
};

class Engine
{
public:
  PATATA_API Engine (void) = default;
  
  PATATA_API Engine (const Patata::EngineCreateInfo &, bool *);
  /*
   * This constructor takes the EngineCreateInfo structure for the initial
   * project configuration. The second argument returns a false boolean if the
   * engine fails to initialize due to a fatal error. If you don't want to use
   * it, pass a nullptr.
   */
  
  PATATA_API bool Init (const Patata::EngineCreateInfo &);
  /*
   * This constructor takes the EngineCreateInfo structure for the initial
   * project configuration. It returns a false boolean if the engine fails to
   * initialize due to a fatal error.
   */

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
  // Window
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
