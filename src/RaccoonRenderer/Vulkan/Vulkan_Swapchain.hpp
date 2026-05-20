#include <future>
#include <functional>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "Vulkan_SetupLog.hpp"
#include "Message.hpp"

#if PATACHE_LINUX_OR_UNIX
extern std::uint8_t scaleInt;
#endif

// Vulkan_ImageView.cpp
bool CreateImageView (Patache::VulkanBackend &, const Patache::SwapchainInfo &);
// Vulkan_FrameBuffer.cpp
bool CreateFrameBuffer (Patache::VulkanBackend &);
// Vulkan_SincronizationPrimitives.cpp
bool CreateSemaphores (Patache::VulkanBackend &);

bool CreateSwapchain (Patache::Engine * const, Patache::SwapchainInfo &);
void RecreateSwapchain (Patache::Engine * const);
