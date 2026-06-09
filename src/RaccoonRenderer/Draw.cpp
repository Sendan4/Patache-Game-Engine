#include <vulkan/vulkan.h>
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

  if (vulkan.physicalDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      vkCmdBindVertexBuffers (vulkan.pCmd[vulkan.currentFrame], 0U, 1U, &vulkan.stagingBuffer,
                              &triangle->bufferOffset);
    }
  else
    {
      vkCmdBindVertexBuffers (vulkan.pCmd[vulkan.currentFrame], 0U, 1U,
                              &vulkan.pRenderBuffer[vulkan.currentFrame], &triangle->bufferOffset);
    }

  vkCmdDraw (vulkan.pCmd[vulkan.currentFrame], 3U, 1U, 0U, 0U);
}
