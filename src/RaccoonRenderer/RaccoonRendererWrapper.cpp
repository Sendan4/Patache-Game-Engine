// Vulkan Backend -> Raccoon Renderer (Wrapping)
#include "RaccoonRenderer.hpp"

/*
The rendering process of each backend must pass through here,
each one will be called according to the configuration.
THIS MAY CHANGE IN THE FUTURE.
*/
void Patata::Graphics::RaccoonRenderer::BeginRender (void) {
    pVulkanBackend->BeginVulkanRender();
}

void Patata::Graphics::RaccoonRenderer::EndRender (void) {
    pVulkanBackend->EndVulkanRender();
}

void Patata::Graphics::RaccoonRenderer::ClearColor (const float & R, const float & G, const float & B, const float & A) {
    pVulkanBackend->VulkanClearColor(R, G, B, A);
}
