#include <cstdio>
#include <future>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

bool CreateImageView (Patache::VulkanBackend &, const Patache::SwapchainInfo &);

#define PATACHE_ERROR_TEXT_SIZE 64
