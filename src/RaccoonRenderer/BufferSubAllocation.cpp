#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"

bool
Patache::Engine::BufferSubAlloc (Patache::Triangle * const triangle)
{
  assert (triangle != nullptr
          && "Triangle pointer in Patache::Triangle::Bsalloc() cannot be null "
             "pointer (nullptr)");

  static constexpr std::uint_fast64_t size{ sizeof (Patache::Vertex2D) * 3 };

  vulkan.renderResult = vmaCopyMemoryToAllocation (
      vulkan.allocator, triangle->vertex, vulkan.stagingAllocation,
      vulkan.stagingBufferInfo[Patache::VkBufferInfo::eCurrentOffset], size);

  assert (vulkan.renderResult == VK_SUCCESS
          && "vmaCopyMemoryToAllocation() Cannot copy data to a staging buffer");

  if (vulkan.renderResult != VK_SUCCESS)
    {
      return false;
    }

  if (vulkan.physicalDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      triangle->bufferOffset = vulkan.stagingBufferInfo[Patache::VkBufferInfo::eCurrentOffset];
    }
  else
    {
      triangle->bufferOffset = vulkan.renderBufferInfo[Patache::VkBufferInfo::eCurrentOffset];
    }

  vulkan.stagingBufferInfo[Patache::VkBufferInfo::eCurrentOffset] += size;
  vulkan.renderBufferInfo[Patache::VkBufferInfo::eCurrentOffset] += size;

  return true;
}

void
Patache::Engine::BufferSubFree (Patache::Triangle * const triangle)
{
  triangle->bufferOffset = ~0U;
}
