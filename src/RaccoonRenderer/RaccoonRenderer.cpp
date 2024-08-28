#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif

#if defined(_WIN64)
#include <windows.h>
#endif

#include <vulkan/vulkan.hpp>
#include <SDL.h>

#include "RaccoonRenderer.hpp"

/*
Punto de entrada para el motor de renderizado

Aqui puedes iniciar otros backends ademas de Vulkan.
si se llega a tener varios, lo mejor seria iniciarlos
mediante el valor de una configuracion.
*/

Patata::Graphics::RaccoonRenderer::RaccoonRenderer (Patata::Config & Config, SDL_Window * Window, bool & WindowResized)
{
  // Backend Principal
  pVulkanBackend = new Patata::Graphics::RaccoonRenderer::VulkanBackend (Config, Window, WindowResized);
}

#include <fast_io.h>

#include "TerminalColors.hpp"
// Provee una funcion para eliminar un puntero y su vez lo muestra en el stdout
#include "ExitLog.hpp"

Patata::Graphics::RaccoonRenderer::~RaccoonRenderer (void)
{
  Patata::Log::DeleteAndLogPtr ("Deallocate Vulkan Backend", pVulkanBackend);
}
