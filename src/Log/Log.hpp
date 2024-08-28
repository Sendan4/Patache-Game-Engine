#pragma once

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <yaml-cpp/yaml.h>

#include "Config.hpp"

namespace Patata
{
namespace Log
{
void StartMapache (void);
void StartPatataLogInfo (void);
void WindowLog (SDL_Window *);

// Vulkan Setup
void VulkanCheck (const std::string &, const vk::Result &);
void VulkanList (const char * List[], const size_t &, const std::string &);

// Errors
void YamlFileErrorMessage (void);
void FatalErrorMessage (const std::string &, const std::string &, const Patata::Config &);
void ErrorMessage (const std::string &);
void WarningMessage (const std::string &);
}
}
