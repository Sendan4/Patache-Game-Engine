// Vulkan Backend -> Raccoon Renderer (Wrapping)

#include <SDL.h>
#include <vulkan/vulkan.hpp>
#include "RaccoonRenderer.hpp"

/*
The rendering process of each backend must pass through here,
each one will be called according to the configuration.
THIS MAY CHANGE IN THE FUTURE.
*/
void
Patata::Graphics::RaccoonRenderer::BeginRender (SDL_Event & Event)
{
    pVulkanBackend->BeginVulkanRender(Event);
}

void
Patata::Graphics::RaccoonRenderer::EndRender (SDL_Event & Event)
{
    pVulkanBackend->EndVulkanRender(Event);
}

void Patata::Graphics::RaccoonRenderer::ClearColor (const float & R, const float & G, const float & B, const float & A) {
    pVulkanBackend->VulkanClearColor(R, G, B, A);
}
