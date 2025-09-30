#include <cstdio>
#include <future>
#include <functional>

#include <vulkan/vulkan.hpp>

// Patache Engine
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"

bool CreateSemaphore (Patache::VulkanBackend &);
bool CreateFence (Patache::VulkanBackend &);

#define PATACHE_ERROR_TEXT_SIZE 64
