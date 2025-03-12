#pragma once

namespace Patata
{
namespace Log
{
void StartPatataLogInfo (void);

// Vulkan Setup
void VulkanCheck (const char * const, const vk::Result &);
void VulkanList (const char * const List[], const uint32_t &,
                 const char * const);

// Errors
void FatalErrorMessage (const char * const, const char * const,
                        const Patata::Config &);
void ErrorMessage (const char * const);
void WarningMessage (const char * const);
}
}
