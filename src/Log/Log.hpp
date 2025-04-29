#pragma once

namespace Patache
{
namespace Log
{
void StartPatacheLogInfo (void);

// Vulkan Setup
void VulkanCheck (const char * const, const vk::Result &);
void VulkanList (const char * const List[], const uint32_t &,
                 const char * const);

// Errors
void FatalErrorMessage (const char * const, const char * const,
                        const Patache::Config &);
void ErrorMessage (const char * const);
void WarningMessage (const char * const);
}
}
