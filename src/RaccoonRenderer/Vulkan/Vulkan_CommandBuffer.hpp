#include <future>
#include <cstdio>

#include <vulkan/vulkan.hpp>

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

bool CreateCommandBuffer (Patache::VulkanBackend &);

#define PATACHE_ERROR_TEXT_SIZE 64
