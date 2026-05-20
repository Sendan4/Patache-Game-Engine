#include <cstdio>
#include <future>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

#if PATACHE_LINUX_OR_UNIX
extern std::uint8_t scaleInt;
#endif

bool CreateFrameBuffer (Patache::VulkanBackend &);

#define PATACHE_ERROR_TEXT_SIZE 64
