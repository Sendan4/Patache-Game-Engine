#pragma once

namespace Patata
{
namespace Log
{
void StartMapache (void);
void StartPatataLogInfo (void);
#if defined(DEBUG)
void WindowLog (SDL_Window *, Patata::EngineInfo *);
#else
void WindowLog (SDL_Window *);
#endif

// Vulkan Setup
void VulkanCheck (const std::string_view &, const vk::Result &);
void VulkanList (const char * List[], const uint32_t &,
                 const std::string_view &);

// Errors
void FatalErrorMessage (const std::string_view &, const std::string_view &,
                        const Patata::Config &);
void ErrorMessage (const std::string_view &);
void WarningMessage (const std::string_view &);
}
}
