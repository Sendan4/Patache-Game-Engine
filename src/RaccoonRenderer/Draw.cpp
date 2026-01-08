#include <vulkan/vulkan.hpp>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"

void
Patache::Engine::Draw (Patache::Triangle * const triangle)
{
  assert (triangle->bufferOffset <= ~0U && "This triangle has been already released");

  assert (
      triangle != nullptr
      && "Patache engine pointer in Patache::Triangle::Draw() cannot be null pointer (nullptr)");

  if (vulkan.physicalDeviceType != vk::PhysicalDeviceType::eDiscreteGpu)
    {
      vulkan.pCmd[vulkan.currentFrame].bindVertexBuffers (0, 1, &vulkan.stagingBuffer,
                                                          &triangle->bufferOffset);
    }
  else
    {
      vulkan.pCmd[vulkan.currentFrame].bindVertexBuffers (
          0, 1, &vulkan.pRenderBuffer[vulkan.currentFrame], &triangle->bufferOffset);
    }

  vulkan.pCmd[vulkan.currentFrame].draw (3, 1, 0, 0);
}
