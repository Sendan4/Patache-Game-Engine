#include <cstdint>

#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <fast_io.h>

// Patache Engine
#include "PatacheEngine/StructConfig.hpp"
#include "PatacheEngine/VulkanBackend.hpp"
#include "Vulkan_SetupLog.hpp"
#include "Message.hpp"

bool CreateDepthBuffer (Patache::VulkanBackend &, Patache::Config &);
