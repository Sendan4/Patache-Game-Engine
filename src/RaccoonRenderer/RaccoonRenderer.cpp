#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif

#if defined(_WIN64)
#include <windows.h>
#endif

#include <fast_io.h>

// Patata Engine
#include "PatataEngineImpl.hpp"
#include "TerminalColors.hpp"

// Provee una funcion para eliminar un puntero y su vez lo muestra en el stdout
#include "ExitLog.hpp"

/*
Punto de entrada para el motor de renderizado

Aqui puedes iniciar otros backends ademas de Vulkan.
si se llega a tener varios, lo mejor seria iniciarlos
mediante el valor de una configuracion.
*/

Patata::Graphics::RaccoonRenderer::RaccoonRenderer (YAML::Node &  Config,
                                                    SDL_Window * Window)
{
  // Backend Principal
  pVulkanBackend
      = new Patata::Graphics::RaccoonRenderer::VulkanBackend (Window, Config);
}

Patata::Graphics::RaccoonRenderer::~RaccoonRenderer (void)
{
  Patata::Log::DeleteAndLogPtr ("Vulkan Backend", pVulkanBackend);
}
