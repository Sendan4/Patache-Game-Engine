#include <vulkan/vulkan.hpp>
#if defined(DEBUG)
#include <imgui_impl_vulkan.h>
#include <SDL_syswm.h>
#endif
#include <SDL.h>

// Begin Dependencies of RaccoonRenderer.hpp
#if defined(DEBUG)
#include "StructEngineInfo.hpp"
#endif
#include "PatataEngine/StructClearColor.hpp"
#include "StructConfig.hpp"
#include "VulkanBackend.hpp"
// End Dependencies of RaccoonRenderer.hpp
#include "RaccoonRenderer.hpp"

/*
Punto de entrada para el motor de renderizado

Aqui puedes iniciar otros backends ademas de Vulkan.
si se llega a tener varios, lo mejor seria iniciarlos
mediante el valor de una configuracion.
*/

Patata::RaccoonRenderer::RaccoonRenderer (
    Patata::RaccoonRendererCreateInfo * RaccoonInfo)
    : pRaccoonInfo (RaccoonInfo)
{
  // Backend Principal
  if (!InitVulkanBackend ())
    return;
}

Patata::RaccoonRenderer::~RaccoonRenderer (void) { CloseVulkanBackend (); }
