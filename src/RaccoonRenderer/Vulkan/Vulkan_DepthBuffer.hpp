#include <future>

#include <vulkan/vulkan.hpp>
#include <fast_io.h>

// Patache Engine
#include "PatacheEngine/StructConfig.hpp"
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"
#include "Message.hpp"

bool CreateDepthBuffer (Patache::VulkanBackend &, Patache::Config &);
