#include <future>
#include <functional>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#if PATACHE_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"

bool CreateSwapChain (Patache::Engine * const, Patache::SwapChainInfo &);
void RecreateSwapChain (Patache::Engine * const);

bool CreateImageView (Patache::VulkanBackend &,
                      const Patache::SwapChainInfo &);
bool CreateFrameBuffer (Patache::VulkanBackend &);
bool CreateSemaphores (Patache::VulkanBackend &);
