#include <cstdio>
#include <future>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

bool CreateFrameBuffer (Patache::VulkanBackend &);

#define PATACHE_ERROR_TEXT_SIZE 64
